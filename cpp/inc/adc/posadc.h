/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef POSADC_H
#define POSADC_H

#include "adc/adc.h"

namespace nq {

class PosADC : public ADC {
  public:
    explicit PosADC();
    PosADC(const PosADC &) = delete;
    virtual ~PosADC() = default;

    float analog_digital_conversion(const float current) const override;

  private:
    float get_max_curr() const;
    float get_min_curr() const;
    const float step_size_; // ADC step size (delta)
};

class ADCUnsigned : public BaseADC {
  public:
    /** Constructor */
    explicit ADCUnsigned();
    ADCUnsigned(const ADCUnsigned &) = delete;
    virtual ~ADCUnsigned() = default;

    /** Convert a vector of analog input currents to digital outputs. */
    virtual void convert(const std::vector<float> &in, std::vector<float> &out,
                         float scale = 1.0, float offset = 0.0) override;

    /** Convert an analog input current to digital output. */
    virtual float convert(const float current, float scale = 1.0,
                          float offset = 0.0) override;

  protected:
    /** Get maximum possible current to ADC */
    virtual float maximum_max_current() override;

    /** Get minimum possible current to ADC */
    virtual float maximum_min_current() override;
};
} // namespace nq

#endif
