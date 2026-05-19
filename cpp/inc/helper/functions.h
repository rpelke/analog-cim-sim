/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "global_vars.h"
#include "xbar/crossbar.h"
#include <cstdint>
#include <vector>

// C interface functions
const void check_pointer(const size_t *const size);
const void check_xbar();
template <typename T>
const uint32_t num_matrix_elems(const std::vector<std::vector<T>> &mat);

#endif