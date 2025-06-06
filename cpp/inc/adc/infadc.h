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

} // namespace nq

#endif
