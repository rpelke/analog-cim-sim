/******************************************************************************
 * Copyright (C) 2026 Joel Klein                                              *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef MAPPING_PROPERTIES_H
#define MAPPING_PROPERTIES_H

#include <cstddef>
#include <cstdint>

#include "helper/definitions.h"

namespace nq {

/** Output encoding of a mapping. */
enum class OutputEnc {
    SINGLE,  /**< One column per output */
    DIFF_COL /**< Difference of two adjacent columns */
};

/** Largest logical matrix that fits on a crossbar. */
struct XbarCapacity {
    uint32_t m; /**< Logical columns */
    uint32_t n; /**< Logical rows */
};

/** Physical cells a single logical weight occupies. */
struct XbarFactors {
    uint32_t col; /**< Cells along the column (M) direction */
    uint32_t row; /**< Cells along the row (N) direction */
};

/** How a mapping places one weight on the crossbar and what it costs. */
struct MappingProperties {
    MappingMode mode; /**< Mode these properties belong to */
    const char *name; /**< Mode name used in the config */
    MappingType type; /**< Mapping family */
    uint32_t
        col_mult; /**< Number of columns per logical column, before SPLIT */
    uint32_t row_mult;  /**< Number of rows per logical row */
    bool split_in_cols; /**< SPLIT segments take extra columns */
    OutputEnc out_enc;  /**< Output encoding */
    uint32_t xbars;     /**< Number of crossbars driven in parallel */
    uint32_t cycles; /**< Number of pulses on the same weights, summed after the
                        ADC */
    bool bit_serial; /**< Inputs applied bit by bit, so scaled by I_BIT */

    /** Whether the mapping stores a second matrix in ia_m_. */
    constexpr bool uses_negative_matrix() const {
        return (col_mult == 2) || (row_mult == 2);
    }

    /** Columns a weight takes for its SPLIT segments, 1 if it does not split.
     */
    constexpr uint32_t split_columns_per_weight(size_t split_size) const {
        return split_in_cols ? static_cast<uint32_t>(split_size) : 1u;
    }

    /** Physical cells a single logical weight occupies. */
    constexpr XbarFactors xbar_factors(size_t split_size) const {
        return {col_mult * split_columns_per_weight(split_size), row_mult};
    }

    /** Largest logical matrix that fits on an M x N crossbar. */
    constexpr XbarCapacity xbar_capacity(size_t split_size, uint32_t M,
                                         uint32_t N) const {
        const XbarFactors factors = xbar_factors(split_size);
        return {M / factors.col, N / factors.row};
    }
};

} // namespace nq

#endif
