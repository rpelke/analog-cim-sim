/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke & Joel Klein                              *
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
    bool is_int_mapping(const MappingMode &mode);
    bool is_bnn_mapping(const MappingMode &mode);
    bool is_tnn_mapping(const MappingMode &mode);
    bool update_from_json(const char *json_string,
                          bool *recreate_xbar = nullptr,
                          const std::vector<std::string> &recreation_keys = {
                              "M", "N", "SPLIT", "digital_only", "HRS", "LRS",
                              "adc_type", "alpha", "resolution", "m_mode",
                              "HRS_NOISE", "LRS_NOISE", "read_disturb",
                              "V_read", "t_read"});
    template <typename T>
    bool update_cfg(const std::string &key, const T &value);

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
    bool read_disturb;
    float V_read;
    float t_read;

  private:
    Config();
    bool apply_config();
    static Config cfg_;
    nlohmann::json cfg_data_;
};

} // namespace nq

#endif
