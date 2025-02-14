/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef ADC_FACTORY_H
#define ADC_FACTORY_H

#include <memory>

#include "adc/adc.h"
#include "adc/symadc.h"
#include "adc/infadc.h"

namespace nq {

enum class ADCType {
    INF_ADC,            // ADC with infinite resolution
    SYM_RANGE_ADC,      // Symmetric range (around 0)
    POS_RANGE_ONLY_ADC  // Positive range only
};

class ADCFactory {
    public:
        static std::unique_ptr<ADC> createADC(ADCType type, const float min_adc_curr, const float max_adc_curr, const float alpha, const uint32_t resolution) {
            switch (type) {
            case ADCType::INF_ADC:
                return std::make_unique<InfADC>();
            case ADCType::SYM_RANGE_ADC:
                return std::make_unique<SymADC>(min_adc_curr, max_adc_curr, alpha, resolution);
            case ADCType::POS_RANGE_ONLY_ADC:
                //TODO: implement PosADC
                return nullptr;
            default:
                return nullptr;
            }
        }
};

} // end namespace

#endif // ADC_FACTORY_H
