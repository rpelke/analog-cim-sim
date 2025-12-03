#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <cstdint>
#include <map>
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

/** Histogram for profiling float arrays with binning. */
class BinnedHistogram {
  public:
    /** Constructor
     *
     * @param min Minimum value to be stored
     * @param max Maximum value to be stored
     * @param bin_size Required bin size when updating histogram
     */
    BinnedHistogram(float min, float max, float bin_size);
    BinnedHistogram() = delete;
    BinnedHistogram(const SimpleHistogram &) = delete;

    /** Destructor */
    virtual ~BinnedHistogram() = default;

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

/** Collection of histograms associated with each operator in a NN
 * workload.
 */
class WorkloadHistograms {
  public:
    WorkloadHistograms();
    WorkloadHistograms(const WorkloadHistograms &) = delete;
    WorkloadHistograms &operator=(const WorkloadHistograms &) = delete;

    /** Destructor */
    virtual ~WorkloadHistograms();

    /** Check if histogram already exists for a layer. */
    bool has_histogram(std::string l_name);

    /** Add a histogram associated with a layer. */
    bool add_histogram(std::string l_name, float min, float max,
                       float bin_size = 1.0);

    /** Get histogram associated with a layer. */
    std::optional<std::reference_wrapper<BinnedHistogram>>
    get_histogram(std::string l_name);

    /** Get histogram data as a JSON object. */
    json to_json();

  protected:
    std::map<std::string, BinnedHistogram> hists_; /**< Layer histograms */
};

/** Singleton collection of histograms profiling ADC inputs.
 */
class ADCHistograms : public WorkloadHistograms {
  public:
    ADCHistograms(const ADCHistograms &) = delete;
    ADCHistograms &operator=(const ADCHistograms &) = delete;

    /** Destructor */
    virtual ~ADCHistograms() override;

    /** Get singleton instance. */
    static ADCHistograms &get_instance();

  private:
    /** Constructor
     *
     * Private constructor for singleton. Can only be accessed with
     * WorkloadHistograms::get_histograms().
     */
    ADCHistograms();
};
} // namespace nq

#endif /* HISTOGRAM_H */
