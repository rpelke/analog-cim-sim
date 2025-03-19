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
#include <memory>
#include <vector>

#include "adc/adcfactory.h"
#include "helper/definitions.h"
#include "mapping/mapper.h"
#include "nlohmann/json.hpp"
#include "xbar/crossbar.h"

#define CFG ::nq::Config::get_cfg()

namespace nq {

class Config {
  public:
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;
    virtual ~Config();

    static Config &get_cfg();
    bool load_cfg(const char *cfg_file);
    bool is_int_mapping();

    uint32_t M;
    uint32_t N;
    std::vector<uint32_t> SPLIT;
    uint32_t W_BIT;
    uint32_t I_BIT;
    bool digital_only;
    float HRS;
    float LRS;
    ADCType adc_type;
    float alpha;
    int32_t resolution;
    MappingMode m_mode;
    bool verbose;
    float HRS_NOISE;
    float LRS_NOISE;

  private:
    Config();
    static Config cfg_;
    nlohmann::json cfg_data_;
};

} // namespace nq

#endif
