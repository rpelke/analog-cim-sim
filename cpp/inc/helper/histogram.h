#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <cstdint>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace nq {

/** Simple histogram for profiling integer arrays. */
class SimpleHistogram {
  public:
    /** Constructor
     *
     * @param min Minimum integer value to be stored
     * @param max Maximum integer value to be stored
     */
    SimpleHistogram(int32_t min, int32_t max);
    SimpleHistogram() = delete;
    SimpleHistogram(const SimpleHistogram &) = delete;

    /** Destructor */
    virtual ~SimpleHistogram() = default;

    /** Update histogram with a vector of values. */
    void update(const std::vector<int32_t> &values);

    /** Get number of samples present in histogram. */
    int64_t get_samples();

    /** Get mean value. */
    float get_mean();

    /** Get variance. */
    float get_variance();

    /** Get histogram data as a JSON object. */
    json to_json();

  private:
    int32_t min_; /**< Minimum value */
    int32_t max_; /**< Maximum value */

    std::vector<int32_t> data_;   /**< Histogram data */
    std::vector<int32_t> values_; /**< Sample values stored in histogram */
};

class Histogram {
  public:
    /** Constructor
     *
     * @param min Minimum value to be stored
     * @param max Maximum value to be stored
     * @param bin_size Required bin size when updating histogram
     */
    Histogram(float min, float max, float bin_size);
    Histogram() = delete;
    Histogram(const SimpleHistogram &) = delete;

    /** Destructor */
    virtual ~Histogram() = default;

    /** Update histogram with a vector of values. */
    void update(const std::vector<float> &values);

    /** Get number of samples present in histogram. */
    int64_t get_samples();

    /** Get mean value. */
    float get_mean();

    /** Get variance. */
    float get_variance();

    /** Get histogram data as a JSON object. */
    json to_json();

  private:
    float min_;      /**< Minimum value */
    float max_;      /**< Maximum value */
    float bin_size_; /**< Bin size */
    float num_bins_; /**< Number of bins */

    std::vector<int32_t> data_; /**< Histogram data */
    std::vector<float> values_; /**< Sample values (mid-point of bins)
                                       stored in histogram */
};
} // namespace nq

#endif /* HISTOGRAM_H */
