/******************************************************************************
 * Copyright (C) 2026 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include <cstdlib>
#include <gtest/gtest.h>
#include <string>

#include "helper/config.h"
#include "inc/test_helper.h"

void check_cfg_fields(const nq::Config &cfg) {
    EXPECT_EQ(cfg.M, 3u);
    EXPECT_EQ(cfg.N, 5u);
    EXPECT_EQ(cfg.m_mode, nq::MappingMode::I_UINT_W_OFFS);
    EXPECT_FALSE(cfg.digital_only);
    EXPECT_EQ(cfg.resolution, 8);
}

// Load config from explicit path
TEST(ConfigTests, LoadCfgFromPath) {
    const std::string cfg_path = get_cfg_file("analog/POS_ADC_1.json");
    nq::Config &cfg = nq::Config::get_cfg();
    ASSERT_TRUE(cfg.load_cfg(cfg_path.c_str()));
    check_cfg_fields(cfg);
}

// Config init: No path provided and no env var set
TEST(ConfigTests, ConfigInitNoPathNoEnvVar) {
    unsetenv("CFG_FILE");
    nq::Config &cfg = nq::Config::get_cfg();
    ASSERT_FALSE(cfg.load_cfg(""));
}

// Trigger error on faulty config path
TEST(ConfigTests, ErrorOnFaultyConfigPath) {
    const std::string cfg_path = get_cfg_file("FAULTY_CFG_PATH.json");
    nq::Config &cfg = nq::Config::get_cfg();
    ASSERT_DEATH(cfg.load_cfg(cfg_path.c_str()), "Could not open config file!");
}

// Load config from environment variable
TEST(ConfigTests, LoadCfgFromEnvVar) {
    const std::string cfg_path = get_cfg_file("analog/POS_ADC_1.json");
    setenv("CFG_FILE", cfg_path.c_str(), /*replace=*/1);
    nq::Config &cfg = nq::Config::get_cfg();
    ASSERT_TRUE(cfg.load_cfg(""));
    check_cfg_fields(cfg);
}
