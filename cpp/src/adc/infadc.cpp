#include "adc/infadc.h"

namespace nq {

InfADC::InfADC() :
    ADC(
        std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(),
        1.0, std::numeric_limits<uint32_t>::max()) {
}

float InfADC::analog_digital_conversion(const float current) const {
    return current;
}

} // end namespace
