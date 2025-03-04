/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef MAPPERINTIV_H
#define MAPPERINTIV_H

#include <cstdint>

#include "mapping/mapper.h"

namespace nq {

// Mapping: I_UINT_W_DIFF
class MapperIntIV : public Mapper {
  public:
    MapperIntIV();
    MapperIntIV(const MapperIntIV &) = delete;
    virtual ~MapperIntIV();

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
