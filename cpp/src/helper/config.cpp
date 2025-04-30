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

bool Config::update_from_json(const char *json_string, bool *recreate_xbar,
                              const std::vector<std::string> &recreation_keys) {
    if (!json_string) {
        std::cerr << "Error: JSON string is null." << std::endl;
        return false;
    }

    try {
        nlohmann::json config_updates = nlohmann::json::parse(json_string);
        bool config_updated = false;

        // Set recreate_xbar to false if provided
        if (recreate_xbar) {
            *recreate_xbar = false;
        }

        // Iterate through all items in the JSON
        for (auto &item : config_updates.items()) {
            const auto &key = item.key();
            auto &value = item.value();

            // First check if the value is actually different
            bool value_changed = false;

            // Check if the key exists and has a different value
            if (cfg_data_.contains(key)) {
                try {
                    if (value.is_number_unsigned()) {
                        uint32_t old_val =
                            getConfigValue<uint32_t>(cfg_data_, key);
                        uint32_t new_val = value.get<uint32_t>();
                        value_changed = (old_val != new_val);
                    } else if (value.is_number_integer()) {
                        int32_t old_val =
                            getConfigValue<int32_t>(cfg_data_, key);
                        int32_t new_val = value.get<int32_t>();
                        value_changed = (old_val != new_val);
                    } else if (value.is_number_float()) {
                        float old_val = getConfigValue<float>(cfg_data_, key);
                        float new_val = value.get<float>();
                        value_changed = (old_val != new_val);
                    } else if (value.is_boolean()) {
                        bool old_val = getConfigValue<bool>(cfg_data_, key);
                        bool new_val = value.get<bool>();
                        value_changed = (old_val != new_val);
                    } else if (value.is_string()) {
                        std::string old_val =
                            getConfigValue<std::string>(cfg_data_, key);
                        std::string new_val = value.get<std::string>();
                        value_changed = (old_val != new_val);
                    } else if (value.is_array()) {
                        try {
                            std::vector<uint32_t> old_array =
                                getConfigValue<std::vector<uint32_t>>(cfg_data_,
                                                                      key);
                            std::vector<uint32_t> new_array =
                                value.get<std::vector<uint32_t>>();
                            value_changed = (old_array != new_array);
                        } catch (const std::exception &e) {
                            // If comparison fails, assume changed
                            value_changed = true;
                        }
                    }
                } catch (const std::exception &e) {
                    // If comparison fails, assume the value has changed
                    value_changed = true;
                    if (verbose) {
                        std::cerr << "Warning: Error comparing values for key "
                                  << key << ": " << e.what() << std::endl;
                    }
                }
            } else {
                // Key doesn't exist, so this is a new value
                value_changed = true;
            }

            // If value hasn't changed, skip to next item
            if (!value_changed) {
                continue;
            }

            // Handle each data type for the update
            bool success = false;
            try {
                if (value.is_number_unsigned()) {
                    success = update_cfg<uint32_t>(key, value.get<uint32_t>());
                } else if (value.is_number_integer()) {
                    success = update_cfg<int32_t>(key, value.get<int32_t>());
                } else if (value.is_number_float()) {
                    success = update_cfg<float>(key, value.get<float>());
                } else if (value.is_boolean()) {
                    success = update_cfg<bool>(key, value.get<bool>());
                } else if (value.is_string()) {
                    success =
                        update_cfg<std::string>(key, value.get<std::string>());
                } else if (value.is_array()) {
                    // Handle array types like SPLIT
                    std::vector<uint32_t> arr_value =
                        value.get<std::vector<uint32_t>>();
                    success = update_cfg<std::vector<uint32_t>>(key, arr_value);
                } else {
                    std::cerr << "Unsupported value type for key: " << key
                              << std::endl;
                    continue;
                }
            } catch (const std::exception &e) {
                std::cerr << "Error updating config value for key " << key
                          << ": " << e.what() << std::endl;
                continue;
            }

            // Check if update was successful
            if (!success) {
                std::cerr << "Failed to update configuration for key: " << key
                          << std::endl;
            } else {
                config_updated = true;

                // Update recreate_xbar flag if this key requires crossbar
                // recreation
                if (recreate_xbar &&
                    std::find(recreation_keys.begin(), recreation_keys.end(),
                              key) != recreation_keys.end()) {
                    *recreate_xbar = true;
                }
            }
        }

        return config_updated;
    } catch (const std::exception &e) {
        std::cerr << "Error updating config from JSON: " << e.what()
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

template <typename T>
bool Config::update_cfg(const std::string &key, const T &value) {
    try {
        cfg_data_[key] = value;
        return apply_config();
    } catch (const std::exception &e) {
        std::cerr << "Error updating JSON config for key: " << key << ": "
                  << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

} // namespace nq
