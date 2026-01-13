/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef MAPPERINTIII_H
#define MAPPERINTIII_H

#include <cstdint>

#include "mapping/mapper.h"

namespace nq {

// Mapping: I_TC_W_DIFF
class MapperIntIII : public Mapper {
  public:
    MapperIntIII();
    MapperIntIII(const MapperIntIII &) = delete;
    virtual ~MapperIntIII();

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
    std::vector<int32_t> tmp_out_int_;
    std::vector<float> tmp_out_fp_;
    std::vector<int32_t> vd_slice_;
};

} // namespace nq

#endif
