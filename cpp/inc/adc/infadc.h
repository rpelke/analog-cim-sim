/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef INFADC_H
#define INFADC_H

#include <cmath>
#include <cstdint>

#include "adc/adc.h"
#include "helper/config.h"

namespace nq {

// ADC with infinite resolution
class InfADC : public ADC {
  public:
    InfADC();
    InfADC(const InfADC &) = delete;
    virtual ~InfADC() = default;

    float analog_digital_conversion(const float current) const override;
};

class ADCInfinite : public BaseADC {
  public:
    /** Constructor */
    explicit ADCInfinite();
    ADCInfinite(const ADCInfinite &) = delete;
    virtual ~ADCInfinite() = default;

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
