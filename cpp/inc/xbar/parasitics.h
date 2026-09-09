/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef PARASITICS_H
#define PARASITICS_H

#include <cstdint>
#include <optional>
#include <vector>

#include "helper/definitions.h"
#include "mapping/mapping_properties.h"

namespace nq {

/** Parasitics solver based on circuit reduction. */
class ParasiticSolver {
  public:
    /** Constructor
     *
     * @param wire_resistance Parasitic wire resistance.
     * @param V_read Read voltage applied to each crossbar row.
     * @param props Properties of the mapping this solver serves.
     */
    ParasiticSolver(const float wire_resistance, const float V_read,
                    const MappingProperties &props);
    ParasiticSolver(const ParasiticSolver &) = delete;
    ParasiticSolver() = delete;

    /** Destructor */
    virtual ~ParasiticSolver() = default;

    /** Set conductance matrix for solver.
     *
     * @param ia Currents matrix
     * @param m_matrix Number of columns in conductance matrix
     * @param n_matrix Number of rows in conductance matrix
     */
    void set_conductance_matrix(std::vector<std::vector<float>> &ia,
                                int32_t m_matrix, int32_t n_matrix);

    /** Set conductance matrix for solver.
     *
     * @param ia_p Positive currents matrix ("plus" mnemonic)
     * @param ia_m Negative currents matrix ("minus" mnemonic)
     * @param m_matrix Number of columns in conductance matrix
     * @param n_matrix Number of rows in conductance matrix
     */
    void set_conductance_matrix(std::vector<std::vector<float>> &ia_p,
                                std::vector<std::vector<float>> &ia_m,
                                int32_t m_matrix, int32_t n_matrix);

    /** Compute output current with parasitics.
     *
     * This solver considers Topology B from Cross-Sim. Here, IR drops only
     * occur over crossbar columns, and row parasitics are be ignored.
     *
     * @param vd Reference to input vector (digital voltages)
     * @param m_matrix Number of columns in conductance matrix
     * @param n_matrix Number of rows in conductance matrix
     * @param res Reference to output vector (analog currents)
     */
    void compute_currents(std::vector<int32_t> &vd, std::vector<float> &res,
                          int32_t m_matrix, int32_t n_matrix);

    /** Compute output current with parasitics.
     *
     * This solver considers Topology B from Cross-Sim. Here, IR drops only
     * occur over crossbar columns, and row parasitics are be ignored.
     *
     * @param vd_p Reference to positive input vector (digital voltages)
     * @param vd_m Reference to negative input vector (digital voltages)
     * @param m_matrix Number of columns in conductance matrix
     * @param n_matrix Number of rows in conductance matrix
     * @param res Reference to output vector (analog currents)
     */
    void compute_currents(std::vector<int32_t> &vd_p,
                          std::vector<int32_t> &vd_m, std::vector<float> &res,
                          int32_t m_matrix, int32_t n_matrix);

    // Encoding function pointer types for different mappings
    typedef void (ParasiticSolver::*WeightEncFunc)(
        std::vector<std::vector<float>> &,
        std::optional<std::vector<std::vector<float>>> &,
        std::vector<std::vector<float>> &, int32_t,
        int32_t); /**< Weight encoding function pointer type */

    typedef void (ParasiticSolver::*InputEncFunc)(
        std::vector<int32_t> &, std::optional<std::vector<int32_t>> &,
        std::vector<int32_t> &,
        int32_t); /**< Input encoding function pointer type */

    typedef void (ParasiticSolver::*OutputEncFunc)(
        std::vector<float> &, std::vector<float> &,
        int32_t); /**< Output encoding function pointer type */

  private:
    ///////////////////////////////
    // Weight encoding functions //
    ///////////////////////////////

    // Functions for different ways to encode the weights matrix for
    // different mappings on the crossbar. Used when setting the conductance
    // matrix.

    /** Encode weight conductance in single cell.
     *
     * Used for BNN III and BNN IV. NOTE: Only ga_p is used for setting
     * conductance.
     */
    void w_enc_single(std::vector<std::vector<float>> &ga_p,
                      std::optional<std::vector<std::vector<float>>> &ga_m,
                      std::vector<std::vector<float>> &ga, int32_t m_matrix,
                      int32_t n_matrix);

    /** Encode weight conductances across 2 columns.
     *
     * Used in BNN I, BNN II, TNN II, TNN III, TNN IV and TNN V. ga_p and ga_m
     * are interleaved in adjacent columns.
     */
    void w_enc_double_col(std::vector<std::vector<float>> &ga_p,
                          std::optional<std::vector<std::vector<float>>> &ga_m,
                          std::vector<std::vector<float>> &ga, int32_t m_matrix,
                          int32_t n_matrix);

    /** Encode weight conductances across 2 rows.
     *
     * Used in BNN V. ia_p and ia_m are interleaved in adjacent rows.
     */
    void w_enc_double_row(std::vector<std::vector<float>> &ga_p,
                          std::optional<std::vector<std::vector<float>>> &ga_m,
                          std::vector<std::vector<float>> &ga, int32_t m_matrix,
                          int32_t n_matrix);

    /** Encode weights across 4 cells.
     *
     * Used in BNN VI and TNN I. ia_p and ia_m are diagonally duplicated across
     * 2 rows and 2 columns. ga_p in (0,0) and (1,1), ga_m in (0,1) and (1,0).
     */
    void w_enc_quad(std::vector<std::vector<float>> &ga_p,
                    std::optional<std::vector<std::vector<float>>> &ga_m,
                    std::vector<std::vector<float>> &ga, int32_t m_matrix,
                    int32_t n_matrix);

    //////////////////////////////
    // Input encoding functions //
    //////////////////////////////

    // Functions for different ways to encode the input vectors for different
    // mappings on the crossbar. Used prior to computing the column currents.

    /** Encode one input per row.
     *
     * NOTE: Only vd_p is used here.
     */
    void i_enc_single(std::vector<int32_t> &vd_p,
                      std::optional<std::vector<int32_t>> &vd_m,
                      std::vector<int32_t> &vd, int32_t n_matrix);

    /** Encode inputs across 2 rows.
     *
     * Here vd_p and vd_m are interleaved across adjacent rows.
     */
    void i_enc_double(std::vector<int32_t> &vd_p,
                      std::optional<std::vector<int32_t>> &vd_m,
                      std::vector<int32_t> &vd, int32_t n_matrix);

    ///////////////////////////////
    // Output encoding functions //
    ///////////////////////////////

    // Functions for different ways to encode the output vector for different
    // mappings on the crossbar. Used after computing column currents (before
    // ADC and compensation).

    /** Encode outputs as one per column. */
    void o_enc_single(std::vector<float> &tmp_res, std::vector<float> &res,
                      int32_t m_matrix);

    /** Encode outputs as difference of two adjacent columns. */
    void o_enc_double(std::vector<float> &tmp_res, std::vector<float> &res,
                      int32_t m_matrix);

    uint32_t m_xbar_; /**< Crossbar columns (bitlines) */
    uint32_t n_xbar_; /**< Crossbar rows (wordlines) */
    float w_res_;     /**< Parasitic wire resistance (in Ohms)*/
    float v_read_;    /**< Crossbar read voltage (in Volts)*/

    std::vector<std::vector<float>> ga_mat_; /**< Conductance matrix (in uS)*/
    std::vector<std::vector<float>>
        ga_mat_gated_;           /**< Gated conductance matrix (in uS)*/
    std::vector<int32_t> vd_;    /**< Digital input voltages */
    std::vector<float> ga_wire_; /**< Per-column parasitic wire conductances */
    std::vector<float> ga_col_acc_; /**< Accumulated column conductances */

    WeightEncFunc weight_enc_func_; /**< Weight encoding function pointer */
    InputEncFunc input_enc_func_;   /**< Input encoding function pointer */
    OutputEncFunc output_enc_func_; /**< Output encoding function pointer */
};

} // namespace nq

#endif /* PARASITICS_H */
