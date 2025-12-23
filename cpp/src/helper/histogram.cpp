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

int64_t SimpleHistogram::get_samples() {
    return std::reduce(std::execution::par, this->data_.begin(),
                       this->data_.end(), int64_t(0), std::plus<int64_t>());
}

float SimpleHistogram::get_mean() {
    return std::transform_reduce(std::execution::par, this->data_.begin(),
                                 this->data_.end(), this->values_.begin(), 0.0,
                                 std::plus<float>(), std::multiplies<float>()) /
           this->get_samples();
}

float SimpleHistogram::get_variance() {
    float mean = get_mean();
    return std::transform_reduce(
        std::execution::par, this->data_.begin(), this->data_.end(),
        this->values_.begin(), 0.0, std::plus<float>(),
        [mean](int32_t d, int32_t v) { return (v * std::pow(d - mean, 2)); });
}

json SimpleHistogram::to_json() {
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

int64_t BinnedHistogram::get_samples() {
    return std::reduce(std::execution::par, this->data_.begin(),
                       this->data_.end(), int64_t(0), std::plus<int64_t>());
}

float BinnedHistogram::get_mean() {
    return std::transform_reduce(std::execution::par, this->data_.begin(),
                                 this->data_.end(), this->values_.begin(), 0.0,
                                 std::plus<float>(), std::multiplies<float>()) /
           this->get_samples();
}

float BinnedHistogram::get_variance() {
    float mean = get_mean();
    return std::transform_reduce(
        std::execution::par, this->data_.begin(), this->data_.end(),
        this->values_.begin(), 0.0, std::plus<float>(),
        [mean](int32_t d, int32_t v) { return (v * std::pow(d - mean, 2)); });
}

json BinnedHistogram::to_json() {
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

WorkloadHistograms::WorkloadHistograms() {}

WorkloadHistograms::~WorkloadHistograms() {}

bool WorkloadHistograms::has_histogram(std::string l_name) {
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

json WorkloadHistograms::to_json() {
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
