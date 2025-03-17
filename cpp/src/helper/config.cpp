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

template <typename T>
T getConfigValue(const nlohmann::json &cfg, const std::string &key) {
    try {
        return cfg.at(key).get<T>();
    } catch (const std::exception &e) {
        std::cerr << "Missing/faulty parameter: '" << key << "' in config."
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

bool Config::load_cfg(const char *cfg_file = "") {
    if (strcmp(cfg_file, "") == 0) {
        const char *cfg_file = std::getenv("CFG_FILE");
        if (cfg_file == nullptr) {
            return false;
        }
        std::ifstream file_stream(cfg_file);
        if (!file_stream.is_open()) {
            std::cerr << "Could not open config file!";
            std::exit(EXIT_FAILURE);
        }
        file_stream >> cfg_data_;
        file_stream.close();
    } else {
        std::ifstream file_stream(cfg_file);
        if (!file_stream.is_open()) {
            std::cerr << "Could not open config file!";
            std::exit(EXIT_FAILURE);
        }
        file_stream >> cfg_data_;
        file_stream.close();
    }

    M = getConfigValue<uint32_t>(cfg_data_, "M");
    N = getConfigValue<uint32_t>(cfg_data_, "N");
    W_BIT = getConfigValue<uint32_t>(cfg_data_, "W_BIT");
    I_BIT = getConfigValue<uint32_t>(cfg_data_, "I_BIT");
    digital_only = getConfigValue<bool>(cfg_data_, "digital_only");
    HRS = getConfigValue<float>(cfg_data_, "HRS");
    LRS = getConfigValue<float>(cfg_data_, "LRS");
    SPLIT = getConfigValue<std::vector<uint32_t>>(cfg_data_, "SPLIT");

    if ((M <= 0) || (N <= 0) || (W_BIT <= 0) || (I_BIT <= 0) || (HRS <= 0.0) ||
        (LRS <= 0.0)) {
        std::cerr << "Error in config parameters." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string adc_type_name =
        getConfigValue<std::string>(cfg_data_, "adc_type");
    if (adc_type_name == "INF_ADC") {
        adc_type = ADCType::INF_ADC;
    } else if (adc_type_name == "SYM_RANGE_ADC") {
        adc_type = ADCType::SYM_RANGE_ADC;
    } else if (adc_type_name == "POS_RANGE_ONLY_ADC") {
        adc_type = ADCType::POS_RANGE_ONLY_ADC;
    } else {
        std::cerr << "Unkown ADC type." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    try {
        alpha = cfg_data_["alpha"];
    } catch (const std::exception &e) {
        std::cerr << "Missing/faulty alpha parameter in config." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    resolution = getConfigValue<uint32_t>(cfg_data_, "resolution");
    verbose = getConfigValue<bool>(cfg_data_, "verbose");

    std::string m_mode_name = getConfigValue<std::string>(cfg_data_, "m_mode");
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
    } else if (m_mode_name == "BNN_VI") {
        m_mode = MappingMode::BNN_VI;
    } else {
        std::cerr << "Unkown MappingMode." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if ((m_mode == MappingMode::I_UINT_W_OFFS) ||
        (m_mode == MappingMode::BNN_III) || (m_mode == MappingMode::BNN_IV) ||
        (m_mode == MappingMode::BNN_V)) {
        if (!((adc_type == ADCType::INF_ADC) ||
              (adc_type == ADCType::POS_RANGE_ONLY_ADC))) {
            std::cerr << "I_UINT_W_OFFS, BNN_III, BNN_IV, BNN_V needs "
                         "INF_ADC or POS_RANGE_ONLY_ADC."
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
    } else if ((m_mode == MappingMode::I_DIFF_W_DIFF_1XB) ||
               (m_mode == MappingMode::I_DIFF_W_DIFF_2XB) ||
               (m_mode == MappingMode::I_OFFS_W_DIFF) ||
               (m_mode == MappingMode::I_TC_W_DIFF) ||
               (m_mode == MappingMode::I_UINT_W_DIFF) ||
               (m_mode == MappingMode::BNN_I) ||
               (m_mode == MappingMode::BNN_II) ||
               (m_mode == MappingMode::BNN_VI)) {
        if (!((adc_type == ADCType::INF_ADC) ||
              (adc_type == ADCType::SYM_RANGE_ADC))) {
            std::cerr << "I_DIFF_W_DIFF_1XB, I_DIFF_W_DIFF_2XB, I_TC_W_DIFF, "
                         "I_TC_W_DIFF, I_UINT_W_DIFF, BNN_I, BNN_II, BNN_VI "
                         "need INF_ADC or "
                         "SYM_RANGE_ADC."
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
    } else {
        std::cerr << "Unkown MappingMode." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Noise of a state is modeles as a Gaussian noise with mean 0
    // The standard deviation is NOISE
    if (!digital_only) {
        NOISE = getConfigValue<float>(cfg_data_, "NOISE");
    }

    return true;
}

} // namespace nq
