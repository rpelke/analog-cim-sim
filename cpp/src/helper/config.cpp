/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke & Joel Klein                              *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "helper/config.h"
#include <optional>

namespace nq {

Config::~Config() {}

Config::Config() {}

Config &Config::get_cfg() {
    static Config instance;
    return instance;
}

// Read parameter from JSON config file
// If the parameter is not found, return the default value if provided.
// Otherwise: exit with an error message.
template <typename T>
T getConfigValue(const nlohmann::json &cfg, const std::string &key,
                 std::optional<T> default_value = std::nullopt) {
    try {
        return cfg.at(key).get<T>();
    } catch (const std::exception &e) {
        if (!default_value.has_value()) {
            std::cerr << "Parameter '" << key
                      << "' not found in config. Add it to the config or "
                         "specify a default value.\n";
            std::exit(EXIT_FAILURE);
        }
        return default_value.value();
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

    return apply_config();
}

bool Config::apply_config() {
    try {
        M = getConfigValue<uint32_t>(cfg_data_, "M");
        N = getConfigValue<uint32_t>(cfg_data_, "N");
        if ((M <= 0) || (N <= 0)) {
            std::cerr << "Error in crossbar dimension." << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::string m_mode_name =
            getConfigValue<std::string>(cfg_data_, "m_mode");
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
        } else if (m_mode_name == "TNN_I") {
            m_mode = MappingMode::TNN_I;
        } else if (m_mode_name == "TNN_II") {
            m_mode = MappingMode::TNN_II;
        } else if (m_mode_name == "TNN_III") {
            m_mode = MappingMode::TNN_III;
        } else if (m_mode_name == "TNN_IV") {
            m_mode = MappingMode::TNN_IV;
        } else if (m_mode_name == "TNN_V") {
            m_mode = MappingMode::TNN_V;
        } else {
            std::cerr << "Unkown MappingMode." << std::endl;
            std::exit(EXIT_FAILURE);
        }

        digital_only = getConfigValue<bool>(cfg_data_, "digital_only");
        if (!digital_only) {
            HRS = getConfigValue<float>(cfg_data_, "HRS");
            LRS = getConfigValue<float>(cfg_data_, "LRS");

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

            if ((HRS <= 0.0) || (LRS <= 0.0)) {
                std::cerr << "Error in config parameters." << std::endl;
                std::exit(EXIT_FAILURE);
            }

            if (adc_type != ADCType::INF_ADC) {
                alpha = getConfigValue<float>(cfg_data_, "alpha");
                resolution = getConfigValue<int32_t>(cfg_data_, "resolution");
                if ((resolution == -1) && (adc_type != ADCType::INF_ADC)) {
                    std::cerr << "ADC resolution is -1. INF_ADC expected"
                              << std::endl;
                    std::exit(EXIT_FAILURE);
                }
            }

            if ((m_mode == MappingMode::I_UINT_W_OFFS) ||
                (m_mode == MappingMode::BNN_III) ||
                (m_mode == MappingMode::BNN_IV) ||
                (m_mode == MappingMode::BNN_V) ||
                (m_mode == MappingMode::TNN_IV) ||
                (m_mode == MappingMode::TNN_V)) {
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
                       (m_mode == MappingMode::BNN_VI) ||
                       (m_mode == MappingMode::TNN_I) ||
                       (m_mode == MappingMode::TNN_II) ||
                       (m_mode == MappingMode::TNN_III)) {
                if (!((adc_type == ADCType::INF_ADC) ||
                      (adc_type == ADCType::SYM_RANGE_ADC))) {
                    std::cerr
                        << "I_DIFF_W_DIFF_1XB, I_DIFF_W_DIFF_2XB, I_TC_W_DIFF, "
                           "I_TC_W_DIFF, I_UINT_W_DIFF, BNN_I, BNN_II, BNN_VI, "
                           "TNN_I, TNN_II need INF_ADC or SYM_RANGE_ADC."
                        << std::endl;
                    std::exit(EXIT_FAILURE);
                }
            } else {
                std::cerr << "Unkown MappingMode." << std::endl;
                std::exit(EXIT_FAILURE);
            }

            // Noise of a state is modeled as a Gaussian noise with mean 0
            // The standard deviation is HRS_NOISE for HRS and LRS_NOISE for LRS
            HRS_NOISE = getConfigValue<float>(cfg_data_, "HRS_NOISE");
            LRS_NOISE = getConfigValue<float>(cfg_data_, "LRS_NOISE");

            // Read disturb
            read_disturb =
                getConfigValue<bool>(cfg_data_, "read_disturb", false);
            if (read_disturb) {
                if (is_int_mapping(m_mode)) {
                    std::cerr
                        << "read_disturb is not supported for int mapping."
                        << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                V_read = getConfigValue<float>(cfg_data_, "V_read");
                if (V_read >= 0.0) {
                    std::cerr << "The implemented read disturb model requires "
                                 "a negative V_read voltage."
                              << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                t_read = getConfigValue<float>(cfg_data_, "t_read");
                read_disturb_update_freq = getConfigValue<uint32_t>(
                    cfg_data_, "read_disturb_update_freq", 1);

                // Read disturb mitigation
                read_disturb_mitigation = getConfigValue<bool>(
                    cfg_data_, "read_disturb_mitigation", false);
                if (read_disturb_mitigation) {
                    read_disturb_mitigation_fp = getConfigValue<float>(
                        cfg_data_, "read_disturb_mitigation_fp");
                    if (read_disturb_mitigation_fp < 1.0) {
                        std::cerr
                            << "read_disturb_mitigation_fp must be >= 1.0."
                            << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                }
            }
        }

        if (is_int_mapping(m_mode)) {
            W_BIT = getConfigValue<uint32_t>(cfg_data_, "W_BIT");
            I_BIT = getConfigValue<uint32_t>(cfg_data_, "I_BIT");
            SPLIT = getConfigValue<std::vector<uint32_t>>(cfg_data_, "SPLIT");

            if ((W_BIT <= 0) || (I_BIT <= 0)) {
                std::cerr << "Error in config parameters." << std::endl;
                std::exit(EXIT_FAILURE);
            }
        } else if ((m_mode == MappingMode::TNN_IV) ||
                   (m_mode == MappingMode::TNN_V)) {
            W_BIT = getConfigValue<uint32_t>(cfg_data_, "W_BIT");
            SPLIT = getConfigValue<std::vector<uint32_t>>(cfg_data_, "SPLIT");
            if (W_BIT != 2) {
                std::cerr << "Error in config parameters." << std::endl;
                std::exit(EXIT_FAILURE);
            }
            if (SPLIT != std::vector<uint32_t>{1, 1}) {
                std::cerr << "SPLIT must be {1, 1} for TNN_IV and TNN_V."
                          << std::endl;
                std::exit(EXIT_FAILURE);
            }
        } else {
            SPLIT = std::vector<uint32_t>{0};
        }

        verbose = getConfigValue<bool>(cfg_data_, "verbose");

        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error applying configuration: " << e.what() << std::endl;
        return false;
    }
}

bool Config::is_int_mapping(const MappingMode &mode) {
    return mode_to_type.at(mode) == MappingType::INT;
}

bool Config::is_bnn_mapping(const MappingMode &mode) {
    return mode_to_type.at(mode) == MappingType::BNN;
}

bool Config::is_tnn_mapping(const MappingMode &mode) {
    return mode_to_type.at(mode) == MappingType::TNN;
}

bool Config::update_cfg(const char *json_string, bool *recreate_xbar,
                        const std::vector<std::string> &recreation_keys) {
    if (!json_string) {
        std::cerr << "Error: JSON string is null." << std::endl;
        return false;
    }

    try {
        // Parse the JSON string
        nlohmann::json updates = nlohmann::json::parse(json_string);

        // Track if configuration was actually modified
        bool config_modified = false;

        // Initialize recreate_xbar flag if provided
        if (recreate_xbar) {
            *recreate_xbar = false;
        }

        // Apply updates to the configuration
        for (const auto &[key, new_value] : updates.items()) {
            // Skip if this key isn't in our config and isn't valid to add
            if (!cfg_data_.contains(key) && new_value.is_null()) {
                continue;
            }

            // Check if the value is actually different from current value
            bool value_changed = false;

            // Compare with existing value if the key already exists
            if (cfg_data_.contains(key)) {
                const auto &current_value = cfg_data_[key];
                value_changed = (current_value != new_value);
            } else {
                // New key being added
                value_changed = true;
            }

            // Only update if value actually changed
            if (value_changed) {
                // Update the config data
                cfg_data_[key] = new_value;
                config_modified = true;

                // Check if this key requires crossbar recreation
                if (recreate_xbar &&
                    std::find(recreation_keys.begin(), recreation_keys.end(),
                              key) != recreation_keys.end()) {
                    *recreate_xbar = true;
                }

                if (verbose) {
                    std::cout << "Updated configuration key: " << key
                              << std::endl;
                }
            }
        }

        // Apply configuration only if changes were made
        if (config_modified) {
            return apply_config();
        }

        return false;
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (const std::exception &e) {
        std::cerr << "Error updating config from JSON: " << e.what()
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

} // namespace nq
