/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/adcfactory.h"
#include "adc/infadc.h"
#include "adc/posadc.h"
#include "adc/symadc.h"

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

std::unique_ptr<BaseADC> ADCFactory::createADCNew(ADCType type) {
    switch (type) {
    case ADCType::INF_ADC:
        return std::make_unique<ADCInfinite>();
    case ADCType::SYM_RANGE_ADC:
        return std::make_unique<ADCSigned>();
    case ADCType::POS_RANGE_ONLY_ADC:
        return std::make_unique<ADCUnsigned>();
    default:
        return nullptr;
    }
}

} // namespace nq
