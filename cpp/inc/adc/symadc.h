/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef SYMADC_H
#define SYMADC_H

#include "adc/posadc.h"

namespace nq {

class SymADC : public ADC {
  public:
    explicit SymADC();
    SymADC(const SymADC &) = delete;
    virtual ~SymADC() = default;

    float analog_digital_conversion(const float current) const override;

  private:
    float get_max_curr() const;
    float get_min_curr() const;
    const float step_size_; // ADC step size (delta)
};

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

} // namespace nq

#endif
