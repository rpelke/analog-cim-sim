/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#ifndef ADC_H
#define ADC_H

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "helper/histogram.h"

namespace nq {

/** Calibration modes for ADC */
enum ADCCalibMode { MAX, CALIB };

/** ADC types */
enum class ADCType {
    INF_ADC,           // ADC with infinite resolution (ADCInfinite)
    SYM_RANGE_ADC,     // Symmetric range (around 0) (ADCSigned)
    POS_RANGE_ONLY_ADC // Positive range only (ADCUnsigned)
};

/** Base ADC class */
class ADC {
  public:
    /** Constructor */
    explicit ADC();
    ADC(const ADC &) = delete;
    virtual ~ADC() = default;

    /** Convert a vector of analog input currents to digital outputs. */
    virtual void convert(const std::vector<float> &in, std::vector<float> &out,
                         const int32_t len, float scale = 1.0,
                         float offset = 0.0, const char *l_name = "Unknown");

    /** Convert an analog input current to digital output. */
    virtual float convert(const float current, float scale = 1.0,
                          float offset = 0.0) = 0;

  protected:
    /** Set maximum and minimum currents to the ADC. */
    void calibrate_currents();

    /** Clip input currents vector for ADC ranges. */
    float clip(float current);

    /** Profile ADC inputs using histograms. */
    void profile_inputs(const std::vector<float> &in, const int32_t len,
                        const char *l_name);

    /** Get maximum possible current to ADC */
    virtual float maximum_max_current() = 0;

    /** Get minimum possible current to ADC */
    virtual float maximum_min_current() = 0;

    int32_t resolution_; /**< ADC resolution (number of bits) */
    float max_curr_;     /**< Maximum current to ADC */
    float min_curr_;     /**< Minimum current to ADC */
    float curr_range_;   /**< Current range sensed by ADC */
    int32_t steps_;      /**< Number of quantization steps */
    std::reference_wrapper<ADCHistograms>
        hists_; /**< Reference to singleton ADC input histograms */
};

/** Ideal ADC with infinite resolution (no clipping/quantization). */
class ADCInfinite : public ADC {
  public:
    /** Constructor */
    explicit ADCInfinite();
    ADCInfinite(const ADCInfinite &) = delete;
    virtual ~ADCInfinite() = default;

    /** Convert an analog input current to digital output. */
    virtual float convert(const float current, float scale = 1.0,
                          float offset = 0.0) override;

  protected:
    /** Get maximum possible current to ADC */
    virtual float maximum_max_current() override;

    /** Get minimum possible current to ADC */
    virtual float maximum_min_current() override;
};

/** Unsigned ADC */
class ADCUnsigned : public ADC {
  public:
    /** Constructor */
    explicit ADCUnsigned();
    ADCUnsigned(const ADCUnsigned &) = delete;
    virtual ~ADCUnsigned() = default;

    /** Convert an analog input current to digital output. */
    virtual float convert(const float current, float scale = 1.0,
                          float offset = 0.0) override;

  protected:
    /** Get maximum possible current to ADC */
    virtual float maximum_max_current() override;

    /** Get minimum possible current to ADC */
    virtual float maximum_min_current() override;
};

/** Signed ADC */
class ADCSigned : public ADCUnsigned {
  public:
    /** Constructor */
    explicit ADCSigned();
    ADCSigned(const ADCSigned &) = delete;
    virtual ~ADCSigned() = default;

  protected:
    /** Get maximum possible current to ADC */
    virtual float maximum_max_current() override;

    /** Get minimum possible current to ADC */
    virtual float maximum_min_current() override;
};

/** Factory class to create ADC from configured option. */
class ADCFactory {
  public:
    static std::unique_ptr<ADC> createADC(ADCType type);
};

} // namespace nq

#endif /* ADC_H */
