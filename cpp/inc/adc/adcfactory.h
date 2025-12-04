/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef ADC_FACTORY_H
#define ADC_FACTORY_H

#include <memory>

#include "adc/adc.h"

namespace nq {

enum class ADCType {
    INF_ADC,           // ADC with infinite resolution
    SYM_RANGE_ADC,     // Symmetric range (around 0)
    POS_RANGE_ONLY_ADC // Positive range only
};

class ADCFactory {
  public:
    static std::unique_ptr<ADC> createADC(ADCType type);
    static std::unique_ptr<BaseADC> createADCNew(ADCType type);
};

} // namespace nq

#endif // ADC_FACTORY_H
