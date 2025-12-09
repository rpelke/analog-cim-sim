/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef MAPPERTNNV_H
#define MAPPERTNNV_H

#include <cstdint>

#include "mapping/mapper.h"

namespace nq {

// Mapping TNN V: i_NN = v_D^+ - v_D^-, w_NN + 1 = (g_D^1, g_D^0)
class MapperTnnV : public Mapper {
  public:
    MapperTnnV();
    MapperTnnV(const MapperTnnV &) = delete;
    virtual ~MapperTnnV();

    void d_write(const int32_t *mat, int32_t m_matrix,
                 int32_t n_matrix) override;
    void a_write(int32_t m_matrix, int32_t n_matrix) override;
    void d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
               int32_t m_matrix, int32_t n_matrix) override;
    void a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
               int32_t m_matrix, int32_t n_matrix,
               const char *l_name = "Unknown") override;

  private:
    // Temporary data for MVM
    std::vector<int32_t> vd_p_;
    std::vector<int32_t> vd_m_;
    std::vector<float> tmp_out_;
    std::vector<float> tmp_out_lsb_;
    std::vector<float> tmp_out_msb_;
    std::vector<float> tmp_out_fp_;
};

} // namespace nq

#endif
