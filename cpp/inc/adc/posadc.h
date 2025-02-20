/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
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

} // namespace nq

#endif
