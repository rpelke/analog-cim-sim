/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef MAPPERBNNII_H
#define MAPPERBNNII_H

#include <cstdint>

#include "mapping/mapper.h"

namespace nq {

// Mapping BNN II: i_NN = -2 v_D + 1, w_NN = g_D+ - g_D-
class MapperBnnII : public Mapper {
  public:
    MapperBnnII();
    MapperBnnII(const MapperBnnII &) = delete;
    virtual ~MapperBnnII();

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
    std::vector<int32_t> vd_;
    std::vector<float> tmp_out_;
};

} // namespace nq

#endif
