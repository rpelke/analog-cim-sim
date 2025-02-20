/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef ADC_H
#define ADC_H

namespace nq {

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

class ADC {
  public:
    explicit ADC(const float min_curr, const float max_curr);
    ADC() = delete;
    ADC(const ADC &) = delete;
    virtual ~ADC() = default;

    virtual float analog_digital_conversion(const float current) const = 0;
    float clip(const float current) const;

  protected:
    const float min_adc_curr_; // minimum current that can be converted (in uA)
    const float max_adc_curr_; // maximum current that can be converted (in uA)
};

} // namespace nq

#endif
