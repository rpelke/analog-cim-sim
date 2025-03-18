/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef CROSSBAR_H
#define CROSSBAR_H

#include <cstdint>
#include <memory>

#include "mapping/mapper.h"

namespace nq {

class Crossbar {
  public:
    Crossbar();
    Crossbar(const Crossbar &) = delete;
    virtual ~Crossbar();

    void write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
    void mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
             int32_t m_matrix, int32_t n_matrix);
    const std::vector<std::vector<int32_t>> &get_gd_p() const;
    const std::vector<std::vector<int32_t>> &get_gd_m() const;
    const std::vector<std::vector<float>> &get_ia_p() const;
    const std::vector<std::vector<float>> &get_ia_m() const;

  private:
    std::unique_ptr<Mapper> mapper_;
    uint64_t write_xbar_counter_; // Number of write function calls
    uint64_t mvm_counter_;        // Number of MVM function calls
};

} // namespace nq

#endif
