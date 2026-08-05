/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <cstdint>
#include <map>
#include <memory>
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
    int64_t get_samples() const;

    /** Get mean value. */
    float get_mean() const;

    /** Get variance. */
    float get_variance() const;

    /** Get histogram data as a JSON object. */
    json to_json() const;

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
    BinnedHistogram(const BinnedHistogram &) = delete;

    /** Destructor */
    virtual ~BinnedHistogram() = default;

    /** Update histogram with a vector of values. */
    void update(const std::vector<float> &values);

    /** Get number of samples present in histogram. */
    int64_t get_samples() const;

    /** Get mean value. */
    float get_mean() const;

    /** Get variance. */
    float get_variance() const;

    /** Get histogram data as a JSON object. */
    json to_json() const;

  private:
    float min_;      /**< Minimum value */
    float max_;      /**< Maximum value */
    float bin_size_; /**< Bin size */
    float num_bins_; /**< Number of bins */

    std::vector<int32_t> data_; /**< Histogram data */
    std::vector<float> values_; /**< Sample values (mid-point of bins)
                                       stored in histogram */
};

/** A stratum is a collection of categorical values. */
class Stratum {

    /** Private constructor. */
    Stratum(std::map<std::string, float> values);

    std::map<std::string, float>
        values_; /**< Values for each stratum category. */

    /** Friend factory for construction. */
    friend class StratumFactory;

  public:
    /** Copy constructors. */
    Stratum(const Stratum &) = default;
    Stratum(Stratum &&) noexcept = default;
    /** Move operators. */
    Stratum &operator=(const Stratum &) = default;
    Stratum &operator=(Stratum &&) noexcept = default;

    std::map<std::string, float> values() const { return values_; }
    bool operator<(const Stratum &other) const {
        return this->values_ < other.values_;
    }
};

/** Factory class for constructing strata.
 *
 * Quantizes incoming values on bin sizes.
 */
class StratumFactory {
  public:
    /** Constructor */
    StratumFactory(std::map<std::string, float> bin_sizes);
    StratumFactory() = delete;
    StratumFactory(const StratumFactory &) = delete;

    /** Get a stratum.
     *
     * Performs quantization/binning of stratum values before construction.
     */
    std::unique_ptr<Stratum>
    get_stratum(std::map<std::string, float> values) const;

  private:
    std::map<std::string, float>
        bin_sizes_; /**< Binning to be applied to each stratum category. */
};

/** A stratified histogram.
 *
 * Used for profiling continuous values (with binning) for each
 * stratum. A stratum is a collection of catagorical variables which
 * are also binned.
 */
class StratifiedHistogram {
  public:
    /** Constructor */
    StratifiedHistogram(float min, float max, float bin_size);
    StratifiedHistogram(const StratifiedHistogram &) = delete;

    /** Destructor */
    virtual ~StratifiedHistogram() = default;

    /** Update histogram for a given stratum with a vector of values. */
    void update(std::unique_ptr<Stratum> stratum, std::vector<float> &values);

    /** Get strata present in the histogram. */
    std::vector<Stratum> get_strata() const;

    /** Get number of samples present in histogram of a stratum. */
    int64_t get_samples(Stratum &stratum) const;

    /** Get mean value of a stratum. */
    float get_mean(Stratum &stratum) const;

    /** Get variance of a stratum. */
    float get_variance(Stratum &stratum) const;

    /** Get histogram data as a JSON object. */
    json to_json() const;

  private:
    float min_;      /**< Minimum value */
    float max_;      /**< Maximum value */
    float bin_size_; /**< Bin size */

    std::map<Stratum, BinnedHistogram> hists_; /**< Strata histograms. */
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
    bool has_histogram(std::string l_name) const;

    /** Add a histogram associated with a layer. */
    bool add_histogram(std::string l_name, float min, float max,
                       float bin_size = 1.0);

    /** Get histogram associated with a layer. */
    std::optional<std::reference_wrapper<BinnedHistogram>>
    get_histogram(std::string l_name);

    /** Get histogram data as a JSON object. */
    json to_json() const;

    /** Get histogram data as a JSON string. */
    std::string to_json_string() const;

  protected:
    std::map<std::string, BinnedHistogram> hists_; /**< Layer histograms */
};

/** Singleton collection of histograms profiling ADC inputs. */
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
