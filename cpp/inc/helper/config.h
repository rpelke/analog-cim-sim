#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint> 
#include <cstdlib>
#include <vector>
#include <fstream>
#include <iostream>

#include "mapping/crossbar_mapping.h"
#include "adc/adcfactory.h"
#include "nlohmann/json.hpp"

#define CFG ::nq::Config::get_cfg()

namespace nq {

class Config 
{
    public:
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
        virtual ~Config();

        static Config& get_cfg();
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

    private:
        Config();
        static Config cfg_;
        bool cfg_loaded_;
        nlohmann::json cfg_data_;
};  

}

#endif
