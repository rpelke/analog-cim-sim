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
    bool update_cfg(const char *json_string, bool *recreate_xbar = nullptr,
                    const std::vector<std::string> &recreation_keys = {
                        "M", "N", "SPLIT", "digital_only", "HRS", "LRS",
                        "adc_type", "alpha", "resolution", "m_mode",
                        "HRS_NOISE", "LRS_NOISE", "read_disturb", "V_read",
                        "t_read", "read_disturb_update_freq",
                        "read_disturb_mitigation",
                        "read_disturb_mitigation_fp"});

    // Matrix dimensions MxN
    uint32_t M;
    uint32_t N;

    // How a single W_BIT weight is distributed to multiple cells
    std::vector<uint32_t> SPLIT;

    // Bit width of weights and inputs
    uint32_t W_BIT;
    uint32_t I_BIT;

    // No conversion to analog values
    bool digital_only;

    // LRS and HRS current (in uA)
    float HRS;
    float LRS;

    // ADC parameters
    // alpha: clipping factor
    ADCType adc_type;
    float alpha;
    int32_t resolution;

    // Mapping strategy
    MappingMode m_mode;

    // Verbose output
    bool verbose;

    // State variability: standard deviation of a gaussian distribution (in uA)
    float HRS_NOISE;
    float LRS_NOISE;

    // Read disturb parameters
    // V_read: read voltage (in V, negative)
    // t_read: time of a read pulse (in s)
    // read_disturb_update_freq: how often (in number of MVMs) the conductance
    // is updated
    bool read_disturb;
    float V_read;
    float t_read;
    uint32_t read_disturb_update_freq;

    // Read disturb mitigation parameters
    // read_disturb_mitigation: whether to use read disturb mitigation
    // mitigation_fp: fitting parameter for the refresh time (>= 1.0)
    bool read_disturb_mitigation;
    float read_disturb_mitigation_fp;

  private:
    Config();
    bool apply_config();
    static Config cfg_;
    nlohmann::json cfg_data_;
};

} // namespace nq

#endif
