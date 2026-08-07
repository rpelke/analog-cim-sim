/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
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
    static constexpr MappingProperties PROPERTIES = {MappingMode::BNN_II,
                                                     "BNN_II",
                                                     MappingType::BNN,
                                                     2,
                                                     1,
                                                     false,
                                                     OutputEnc::DIFF_COL,
                                                     1,
                                                     1,
                                                     false};

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

    // For BNN II, average cell value is always 0.5, due to
    // differential encoding of binary values.
    const float avg_cell_val_ = 0.5;
};

} // namespace nq

#endif
