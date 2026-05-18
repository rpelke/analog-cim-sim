#pragma once

#include "helper/config.h"
#include "xbar/crossbar.h"

#include "interface_xbar.h"

extern bool cfg_loaded;
extern std::unique_ptr<nq::Crossbar> xbar;
extern std::string adc_profile_cache;
extern std::unique_ptr<tbb::global_control> gc; /** TBB Global Control */