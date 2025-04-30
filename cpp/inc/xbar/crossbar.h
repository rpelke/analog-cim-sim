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
#include "xbar/read_disturb.h"

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
    const std::vector<std::vector<uint64_t>> &get_cycles_p() const;
    const std::vector<std::vector<uint64_t>> &get_cycles_m() const;
    const uint64_t get_write_xbar_counter() const;
    const uint64_t get_mvm_counter() const;
    const uint64_t get_read_num() const;

  private:
    std::unique_ptr<Mapper> mapper_;
    uint64_t write_xbar_counter_; // Number of write function calls
    uint64_t mvm_counter_;        // Number of MVM function calls
    std::shared_ptr<ReadDisturb> rd_model_;
    uint64_t read_num_; // Number of consecutive read operations
};

} // namespace nq

#endif
