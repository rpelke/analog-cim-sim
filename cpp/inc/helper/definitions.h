/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Joel Klein                               *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

namespace nq {

enum class MappingType { BNN, TNN, INT };

enum class MappingMode {
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
    BNN_VI,
    TNN_I,
    TNN_II,
    TNN_III,
    TNN_IV,
    TNN_V,
    NUM_MODES
};

enum class ReadDisturbMitigationStrategy { SOFTWARE, CELL_BASED, OFF };

} // namespace nq

#endif
