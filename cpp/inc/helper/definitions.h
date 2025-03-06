/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

namespace nq {

enum class MappingMode { // Mapping modes for inputs/weights as signed
                         // integers
    I_DIFF_W_DIFF_1XB, // Inputs and weights in differential mode, use 1 XBar /
                       // 2 Cycles
    I_DIFF_W_DIFF_2XB, // Inputs and weights in differential mode, use 2 XBars /
                       // 1 Cycle
    I_OFFS_W_DIFF,     // Inputs shifted to positive range
    I_TC_W_DIFF,       // Inputs interpreted as two's complement
    I_UINT_W_DIFF, // Inputs are already positive-only. Weights in differential
                   // mode
    I_UINT_W_OFFS, // Inputs are already positive-only. Weights use an offset
    BNN_I,
    BNN_II,
    BNN_III,
    BNN_IV,
    BNN_V,
    BNN_VI
};

static std::string m_mode_to_string(MappingMode mode) {
    switch (mode) {
    case MappingMode::I_DIFF_W_DIFF_1XB:
        return "I_DIFF_W_DIFF_1XB";
    case MappingMode::I_DIFF_W_DIFF_2XB:
        return "I_DIFF_W_DIFF_2XB";
    case MappingMode::I_OFFS_W_DIFF:
        return "I_OFFS_W_DIFF";
    case MappingMode::I_TC_W_DIFF:
        return "I_TC_W_DIFF";
    case MappingMode::I_UINT_W_DIFF:
        return "I_UINT_W_DIFF";
    case MappingMode::I_UINT_W_OFFS:
        return "I_UINT_W_OFFS";
    case MappingMode::BNN_I:
        return "BNN_I";
    case MappingMode::BNN_II:
        return "BNN_II";
    case MappingMode::BNN_III:
        return "BNN_III";
    case MappingMode::BNN_IV:
        return "BNN_IV";
    case MappingMode::BNN_V:
        return "BNN_V";
    case MappingMode::BNN_VI:
        return "BNN_VI";
    default:
        return "Unknown mode";
    }
}

} // namespace nq

#endif
