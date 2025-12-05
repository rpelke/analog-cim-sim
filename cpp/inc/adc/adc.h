/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef ADC_H
#define ADC_H

#include <cstdint>
#include <vector>

namespace nq {

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

class ADC {
  public:
    explicit ADC(const float min_curr, const float max_curr);
    ADC() = delete;
    ADC(const ADC &) = delete;
    virtual ~ADC() = default;

    virtual float analog_digital_conversion(const float current) const = 0;
    float clip(const float current) const;

  protected:
    const float min_adc_curr_; // minimum current that can be converted (in uA)
    const float max_adc_curr_; // maximum current that can be converted (in uA)
};

/** Calibration modes for ADC */
enum AdcCalibMode { MAX, CALIB };

/** Base ADC class */
class BaseADC {
  public:
    /** Constructor */
    explicit BaseADC();
    BaseADC(const ADC &) = delete;
    virtual ~BaseADC() = default;

    /** Convert a vector of analog input currents to digital outputs. */
    virtual void convert(const std::vector<float> &in, std::vector<float> &out,
                         float scale = 1.0, float offset = 0.0) = 0;

    /** Convert an analog input current to digital output. */
    virtual float convert(const float current, float scale = 1.0,
                          float offset = 0.0) = 0;

  protected:
    /** Set maximum and minimum currents to the ADC. */
    void calibrate_currents();

    /** Clip input currents vector for ADC ranges. */
    float clip(float current);

    /** Get maximum possible current to ADC */
    virtual float maximum_max_current() = 0;

    /** Get minimum possible current to ADC */
    virtual float maximum_min_current() = 0;

    int32_t resolution_; /**< ADC resolution (number of bits) */
    float max_curr_;     /**< Maximum current to ADC */
    float min_curr_;     /**< Minimum current to ADC */
    float curr_range_;   /**< Current range sensed by ADC */
    int32_t steps_;      /**< Number of quantization steps */
};

} // namespace nq

#endif
