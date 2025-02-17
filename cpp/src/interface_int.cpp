/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <iostream>
#include <stdio.h>
#include <vector>

#include "helper/config.h"

bool load_cfg = nq::Config::get_cfg().load_cfg();

nq::CrossbarMapping xbar(CFG.M, CFG.N, CFG.W_BIT, CFG.I_BIT, CFG.SPLIT,
                         CFG.digital_only, CFG.m_mode, CFG.HRS, CFG.LRS,
                         CFG.adc_type, CFG.min_adc_curr, CFG.max_adc_curr,
                         CFG.alpha, CFG.resolution, CFG.verbose);

extern "C" int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix) {
    xbar.mvm(res, vec, mat, m_matrix, n_matrix);
    return 0;
}

extern "C" int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    xbar.write(mat, m_matrix, n_matrix);
    return 0;
}
