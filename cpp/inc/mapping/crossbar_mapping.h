/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef CROSSBARMAPPING_H
#define CROSSBARMAPPING_H

#include <cstdint>

#include "xbar/analog_crossbar.h"
#include "xbar/digital_crossbar.h"

namespace nq {

class CrossbarMapping {
  public:
    explicit CrossbarMapping();
    CrossbarMapping(const CrossbarMapping &) = delete;
    virtual ~CrossbarMapping();

    void write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
    void mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
             int32_t m_matrix, int32_t n_matrix);

  private:
    DigitalCrossbar d_xbar_;
    AnalogCrossbar a_xbar_;
    uint64_t write_xbar_counter_; // Number of write function calls
    uint64_t mvm_counter_;        // Number of MVM function calls
};

} // namespace nq

#endif
