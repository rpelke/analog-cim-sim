/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "helper/config.h"

namespace nq {

Config::~Config() {}

Config::Config() {}

Config &Config::get_cfg() {
    static Config instance;
    return instance;
}

bool Config::load_cfg(const char *cfg_file = "") {
    if (strcmp(cfg_file, "") == 0) {
        const char *cfg_file = std::getenv("CFG_FILE");
        if (cfg_file == nullptr) {
            return false;
        }
        std::cout << "Load config from " << cfg_file << std::endl;
        std::ifstream file_stream(cfg_file);
        if (!file_stream.is_open()) {
            throw std::runtime_error("Could not open config file!");
        }
        file_stream >> cfg_data_;
        file_stream.close();
    } else {
        std::ifstream file_stream(cfg_file);
        if (!file_stream.is_open()) {
            throw std::runtime_error("Could not open config file!");
        }
        file_stream >> cfg_data_;
        file_stream.close();
    }

    M = cfg_data_["M"];
    N = cfg_data_["N"];
    SPLIT = cfg_data_["SPLIT"].get<std::vector<uint32_t>>();
    W_BIT = cfg_data_["W_BIT"];
    I_BIT = cfg_data_["I_BIT"];
    digital_only = cfg_data_["digital_only"];
    HRS = cfg_data_["HRS"];
    LRS = cfg_data_["LRS"];

    if ((M <= 0) || (N <= 0) || (W_BIT <= 0) || (I_BIT <= 0) || (HRS <= 0.0) ||
        (LRS <= 0.0)) {
        std::cerr << "Error in config parameters." << std::endl;
        abort();
    }

    std::string adc_type_name = cfg_data_["adc_type"].get<std::string>();
    if (adc_type_name == "INF_ADC") {
        adc_type = ADCType::INF_ADC;
    } else if (adc_type_name == "SYM_RANGE_ADC") {
        adc_type = ADCType::SYM_RANGE_ADC;
    } else if (adc_type_name == "POS_RANGE_ONLY_ADC") {
        adc_type = ADCType::POS_RANGE_ONLY_ADC;
    } else {
        std::cerr << "Unkown ADC type." << std::endl;
        abort();
    }

    alpha = cfg_data_["alpha"];
    resolution = cfg_data_["resolution"];
    verbose = cfg_data_["verbose"];

    std::string m_mode_name = cfg_data_["m_mode"].get<std::string>();
    if (m_mode_name == "I_DIFF_W_DIFF_1XB") {
        m_mode = MappingMode::I_DIFF_W_DIFF_1XB;
    } else if (m_mode_name == "I_DIFF_W_DIFF_2XB") {
        m_mode = MappingMode::I_DIFF_W_DIFF_2XB;
    } else if (m_mode_name == "I_OFFS_W_DIFF") {
        m_mode = MappingMode::I_OFFS_W_DIFF;
    } else if (m_mode_name == "I_TC_W_DIFF") {
        m_mode = MappingMode::I_TC_W_DIFF;
    } else if (m_mode_name == "I_UINT_W_DIFF") {
        m_mode = MappingMode::I_UINT_W_DIFF;
    } else if (m_mode_name == "I_UINT_W_OFFS") {
        m_mode = MappingMode::I_UINT_W_OFFS;
    } else if (m_mode_name == "BNN_I") {
        m_mode = MappingMode::BNN_I;
    } else if (m_mode_name == "BNN_II") {
        m_mode = MappingMode::BNN_II;
    } else if (m_mode_name == "BNN_III") {
        m_mode = MappingMode::BNN_III;
    } else if (m_mode_name == "BNN_IV") {
        m_mode = MappingMode::BNN_IV;
    } else if (m_mode_name == "BNN_V") {
        m_mode = MappingMode::BNN_V;
    } else {
        std::cerr << "Unkown MappingMode." << std::endl;
        abort();
    }

    if ((m_mode == MappingMode::I_UINT_W_OFFS) ||
        (m_mode == MappingMode::BNN_III) || (m_mode == MappingMode::BNN_IV) ||
        (m_mode == MappingMode::BNN_V)) {
        if (!((adc_type == ADCType::INF_ADC) ||
              (adc_type == ADCType::POS_RANGE_ONLY_ADC))) {
            throw std::runtime_error(
                "I_UINT_W_OFFS, BNN_III, BNN_IV, BNN_V needs "
                "INF_ADC or POS_RANGE_ONLY_ADC.");
        }
    } else if ((m_mode == MappingMode::I_DIFF_W_DIFF_1XB) ||
               (m_mode == MappingMode::I_DIFF_W_DIFF_2XB) ||
               (m_mode == MappingMode::I_OFFS_W_DIFF) ||
               (m_mode == MappingMode::I_TC_W_DIFF) ||
               (m_mode == MappingMode::I_UINT_W_DIFF) ||
               (m_mode == MappingMode::BNN_I) ||
               (m_mode == MappingMode::BNN_II)) {
        if (!((adc_type == ADCType::INF_ADC) ||
              (adc_type == ADCType::SYM_RANGE_ADC))) {
            std::cerr << "I_DIFF_W_DIFF_1XB, I_DIFF_W_DIFF_2XB, I_TC_W_DIFF, "
                         "I_TC_W_DIFF, I_UINT_W_DIFF, BNN_I need INF_ADC or "
                         "SYM_RANGE_ADC."
                      << std::endl;
            abort();
        }
    } else {
        std::cerr << "Unkown MappingMode." << std::endl;
        abort();
    }

    return true;
}

} // namespace nq
