/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include "helper/histogram.h"

#include <algorithm>
#include <cmath>
#include <execution>
#include <functional>
#include <iterator>
#include <unordered_map>

namespace nq {

SimpleHistogram::SimpleHistogram(int32_t min, int32_t max) :
    min_(min),
    max_(max),
    data_(std::vector<int32_t>(max - min + 1, 0)),
    values_(std::vector<int32_t>(max - min + 1, 0)) {
    // Generate values present in histogram
    std::generate(values_.begin(), values_.end(),
                  [n = this->min_]() mutable { return n++; });
}

void SimpleHistogram::update(const std::vector<int32_t> &values) {
    // TODO: Check if values are within histogram ranges
    // Offset values to get indices to update histogram data
    std::vector<int32_t> indices(values.size(), 0);
    std::transform(std::execution::par_unseq, values.begin(), values.end(),
                   indices.begin(),
                   [this](int32_t v) -> int32_t { return v - this->min_; });
    // Update histogram data
    std::for_each(std::execution::seq, indices.begin(), indices.end(),
                  [this](int32_t i) -> void { this->data_[i]++; });
}

int64_t SimpleHistogram::get_samples() const {
    return std::reduce(std::execution::par, this->data_.begin(),
                       this->data_.end(), int64_t(0), std::plus<int64_t>());
}

float SimpleHistogram::get_mean() const {
    return std::transform_reduce(std::execution::par, this->data_.begin(),
                                 this->data_.end(), this->values_.begin(), 0.0,
                                 std::plus<float>(), std::multiplies<float>()) /
           get_samples();
}

float SimpleHistogram::get_variance() const {
    float mean = get_mean();
    return std::transform_reduce(std::execution::par, this->data_.begin(),
                                 this->data_.end(), this->values_.begin(), 0.0,
                                 std::plus<float>(),
                                 [mean](int32_t d, int32_t v) {
                                     return (d * std::pow(v - mean, 2));
                                 }) /
           get_samples();
}

json SimpleHistogram::to_json() const {
    std::unordered_map<int32_t, int32_t> hist_map;
    std::transform(this->data_.begin(), this->data_.end(),
                   this->values_.begin(),
                   std::inserter(hist_map, hist_map.end()),
                   [](int32_t d, int32_t v) -> std::pair<int32_t, int32_t> {
                       return std::make_pair(v, d);
                   });

    return json{{"hist", hist_map},
                {"samples", get_samples()},
                {"mean", get_mean()},
                {"var", get_variance()}};
}

BinnedHistogram::BinnedHistogram(float min, float max, float bin_size) :
    min_(min),
    max_(max),
    bin_size_(bin_size),
    num_bins_(round((max - min) / bin_size_)),
    data_(std::vector<int32_t>(num_bins_, 0)),
    values_(std::vector<float>(num_bins_, 0)) {
    // Generate values present in histogram
    std::generate(
        values_.begin(), values_.end(),
        [n = this->min_ + this->bin_size_ / 2, p = 0.0, this]() mutable {
            p = n;
            n += this->bin_size_;
            return p;
        });
}

void BinnedHistogram::update(const std::vector<float> &values) {
    // TODO: Check if values are within histogram ranges
    // Offset values to get indices to update histogram data
    std::vector<int32_t> indices(values.size(), 0);
    std::transform(std::execution::par_unseq, values.begin(), values.end(),
                   indices.begin(), [this](float v) -> int32_t {
                       return round((v - this->min_) / bin_size_);
                   });
    // Update histogram data
    std::for_each(std::execution::seq, indices.begin(), indices.end(),
                  [this](int32_t i) -> void { this->data_[i]++; });
}

int64_t BinnedHistogram::get_samples() const {
    return std::reduce(std::execution::par, this->data_.begin(),
                       this->data_.end(), int64_t(0), std::plus<int64_t>());
}

float BinnedHistogram::get_mean() const {
    return std::transform_reduce(std::execution::par, this->data_.begin(),
                                 this->data_.end(), this->values_.begin(), 0.0,
                                 std::plus<float>(), std::multiplies<float>()) /
           get_samples();
}

float BinnedHistogram::get_variance() const {
    float mean = get_mean();
    return std::transform_reduce(std::execution::par, this->data_.begin(),
                                 this->data_.end(), this->values_.begin(), 0.0,
                                 std::plus<float>(),
                                 [mean](int32_t d, int32_t v) {
                                     return (d * std::pow(v - mean, 2));
                                 }) /
           get_samples();
}

json BinnedHistogram::to_json() const {
    std::map<float, int32_t> hist_map;
    std::transform(this->data_.begin(), this->data_.end(),
                   this->values_.begin(),
                   std::inserter(hist_map, hist_map.end()),
                   [](int32_t d, float v) { return std::make_pair(v, d); });

    return json{{"hist", hist_map},
                {"samples", get_samples()},
                {"mean", get_mean()},
                {"var", get_variance()}};
}

StratumFactory::StratumFactory(std::map<std::string, float> bin_sizes) :
    bin_sizes_(bin_sizes) {}

std::unique_ptr<Stratum>
StratumFactory::get_stratum(std::map<std::string, float> values) const {
    std::map<std::string, float> quantized_values;
    std::transform(
        values.begin(), values.end(),
        std::inserter(quantized_values, quantized_values.end()),
        [this](const std::pair<const std::string, float> &kv) {
            auto it = bin_sizes_.find(kv.first);
            float bin_size = (it != bin_sizes_.end()) ? it->second : 1.0f;
            return std::make_pair(kv.first,
                                  std::round(kv.second / bin_size) * bin_size);
        });

    return std::unique_ptr<Stratum>(new Stratum(std::move(quantized_values)));
}

StratifiedHistogram::StratifiedHistogram(float min, float max, float bin_size) :
    min_(min),
    max_(max),
    bin_size_(bin_size) {}

void StratifiedHistogram::update(std::unique_ptr<Stratum> stratum,
                                 std::vector<float> &values) {
    auto it = hists_.find(*stratum);
    if (it == hists_.end()) {
        it = hists_
                 .emplace(std::move(*stratum),
                          BinnedHistogram(min_, max_, bin_size_))
                 .first;
    }
    it->second.update(values);
}

std::vector<Stratum> StratifiedHistogram::get_strata() const {
    std::vector<Stratum> strata;
    strata.reserve(hists_.size());

    std::transform(
        hists_.begin(), hists_.end(), std::back_inserter(strata),
        [](const std::pair<Stratum, BinnedHistogram> &kv) { return kv.first; });

    return strata;
}

int64_t StratifiedHistogram::get_samples(Stratum &stratum) const {
    return hists_.at(stratum).get_samples();
}

float StratifiedHistogram::get_mean(Stratum &stratum) const {
    return hists_.at(stratum).get_mean();
}

float StratifiedHistogram::get_variance(Stratum &stratum) const {
    return hists_.at(stratum).get_variance();
}

json StratifiedHistogram::to_json() const {
    std::vector<json> strata;
    strata.reserve(hists_.size());
    std::transform(hists_.cbegin(), hists_.cend(), std::back_inserter(strata),
                   [](std::pair<const Stratum, BinnedHistogram> &kv) {
                       return json{{"stratum", kv.first.values()},
                                   {"histogram", kv.second.to_json()}};
                   });

    return json{{"strata", strata}};
}

WorkloadHistograms::WorkloadHistograms() {}

WorkloadHistograms::~WorkloadHistograms() {}

bool WorkloadHistograms::has_histogram(std::string l_name) const {
    auto val = hists_.find(l_name);
    return val != hists_.end();
}

bool WorkloadHistograms::add_histogram(std::string l_name, float min, float max,
                                       float bin_size) {
    return hists_.insert({l_name, BinnedHistogram(min, max, bin_size)}).second;
}

std::optional<std::reference_wrapper<BinnedHistogram>>
WorkloadHistograms::get_histogram(std::string l_name) {
    if (auto val = hists_.find(l_name); val != hists_.end()) {
        return std::optional<std::reference_wrapper<BinnedHistogram>>(
            val->second);
    }
    return std::optional<std::reference_wrapper<BinnedHistogram>>();
}

json WorkloadHistograms::to_json() const {
    struct JSONConstructor {
        void operator()(std::pair<std::string, BinnedHistogram> hist) {
            json_obj.emplace(hist.first, hist.second.to_json());
        }
        json json_obj{};
    };
    JSONConstructor json_cons = std::for_each(
        this->hists_.begin(), this->hists_.end(), JSONConstructor());
    return json_cons.json_obj;
}

ADCHistograms::ADCHistograms() {}

ADCHistograms::~ADCHistograms() {}

ADCHistograms &ADCHistograms::get_instance() {
    static ADCHistograms instance;
    return instance;
}

} // namespace nq
