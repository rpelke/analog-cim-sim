/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "adc/adcfactory.h"
#include "mapping/crossbar_mapping.h"
#include "nlohmann/json.hpp"

#define CFG ::nq::Config::get_cfg()

namespace nq {

class Config {
  public:
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;
    virtual ~Config();

    static Config &get_cfg();
    bool load_cfg();

    uint32_t M;
    uint32_t N;
    std::vector<uint32_t> SPLIT;
    uint32_t W_BIT;
    uint32_t I_BIT;
    bool digital_only;
    float HRS;
    float LRS;
    ADCType adc_type;
    float min_adc_curr;
    float max_adc_curr;
    float alpha;
    uint32_t resolution;
    INT8MappingMode m_mode;
    bool verbose;

  private:
    Config();
    static Config cfg_;
    bool cfg_loaded_;
    nlohmann::json cfg_data_;
};

} // namespace nq

#endif
