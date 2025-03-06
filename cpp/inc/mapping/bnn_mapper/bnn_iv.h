/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef MAPPERBNNIV_H
#define MAPPERBNNIV_H

#include <cstdint>

#include "mapping/mapper.h"

namespace nq {

// Mapping BNN IV: i_NN = v_D+ - v_D-, w_NN = - 2 g_D + 1
class MapperBnnIV : public Mapper {
  public:
    MapperBnnIV();
    MapperBnnIV(const MapperBnnIV &) = delete;
    virtual ~MapperBnnIV();

    void d_write(const int32_t *mat, int32_t m_matrix,
                 int32_t n_matrix) override;
    void a_write(int32_t m_matrix, int32_t n_matrix) override;
    void d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
               int32_t m_matrix, int32_t n_matrix) override;
    void a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
               int32_t m_matrix, int32_t n_matrix) override;
};

} // namespace nq

#endif
