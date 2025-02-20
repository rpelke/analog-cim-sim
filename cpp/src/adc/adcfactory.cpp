/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/adcfactory.h"
#include "adc/infadc.h"
#include "adc/symadc.h"
#include "adc/posadc.h"

namespace nq {

std::unique_ptr<ADC> ADCFactory::createADC(ADCType type) {
    switch (type) {
    case ADCType::INF_ADC:
        return std::make_unique<InfADC>();
    case ADCType::SYM_RANGE_ADC:
        return std::make_unique<SymADC>();
    case ADCType::POS_RANGE_ONLY_ADC:
    return std::make_unique<PosADC>();
        return nullptr;
    default:
        return nullptr;
    }
}

} // namespace nq
