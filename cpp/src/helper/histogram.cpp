#include "helper/histogram.h"

#include <algorithm>
#include <cmath>
#include <execution>

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
    std::map<int32_t, int32_t> hist_map;
    std::transform(std::execution::par, this->data_.begin(), this->data_.end(),
                   this->values_.begin(),
                   std::inserter(hist_map, hist_map.end()),
                   [](int32_t d, int32_t v) { return std::make_pair(v, d); });

    return json{{"hist", hist_map},
                {"samples", get_samples()},
                {"mean", get_mean()},
                {"var", get_variance()}};
}

Histogram::Histogram(float min, float max, float bin_size) :
    min_(min),
    max_(max),
    bin_size_(bin_size),
    num_bins_(round((max - min + 1) / bin_size_)),
    data_(std::vector<int32_t>(num_bins_, 0)),
    values_(std::vector<float>(num_bins_, 0)) {
    // Generate values present in histogram
    std::generate(
        values_.begin(), values_.end(),
        [n = this->min_, this]() mutable { return n += this->bin_size_ / 2; });
}

void Histogram::update(const std::vector<float> &values) {
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

int64_t Histogram::get_samples() {
    return std::reduce(std::execution::par, this->data_.begin(),
                       this->data_.end(), int64_t(0), std::plus<int64_t>());
}

float Histogram::get_mean() {
    return std::transform_reduce(std::execution::par, this->data_.begin(),
                                 this->data_.end(), this->values_.begin(), 0.0,
                                 std::plus<float>(), std::multiplies<float>()) /
           this->get_samples();
}

float Histogram::get_variance() {
    float mean = get_mean();
    return std::transform_reduce(
        std::execution::par, this->data_.begin(), this->data_.end(),
        this->values_.begin(), 0.0, std::plus<float>(),
        [mean](int32_t d, int32_t v) { return (v * std::pow(d - mean, 2)); });
}

json Histogram::to_json() {
    std::map<float, int32_t> hist_map;
    std::transform(std::execution::par, this->data_.begin(), this->data_.end(),
                   this->values_.begin(),
                   std::inserter(hist_map, hist_map.end()),
                   [](int32_t d, float v) { return std::make_pair(v, d); });

    return json{{"hist", hist_map},
                {"samples", get_samples()},
                {"mean", get_mean()},
                {"var", get_variance()}};
}
} // namespace nq
