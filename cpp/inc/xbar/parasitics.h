/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef PARASITICS_H
#define PARASITICS_H

#include <cstdint>
#include <vector>

#include "helper/definitions.h"

namespace nq {

/** Parasitics solver based on circuit reduction. */
class ParasiticSolver {
  public:
    /** Constructor
     *
     * @param wire_resistance Parasitic wire resistance.
     * @param V_read Read voltage applied to each crossbar row.
     */
    ParasiticSolver(const float wire_resistance, const float V_read);
    ParasiticSolver(const ParasiticSolver &) = delete;
    ParasiticSolver() = delete;

    /** Destructor */
    virtual ~ParasiticSolver() = default;

    /** Set conductance matrix for solver.
     *
     * @param ia_p Positive currents matrix ("plus" mnemonic)
     * @param ia_m Negative currents matrix ("minus" mnemonic)
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
     * @param res Reference to output vector (analog currents)
     */
    void compute_currents(std::vector<int32_t> &vd, std::vector<float> &res);

  private:
    uint32_t m_xbar;    /**< Crossbar rows */
    uint32_t n_xbar;    /**< Crossbar columns */
    float w_res;        /**< Parasitic wire resistance (in Ohms)*/
    float v_read;       /**< Crossbar read voltage (in Volts)*/
    MappingMode m_mode; /**< Mapping mode */

    std::vector<std::vector<float>> ga_mat; /**< Conductance matrix (in uS)*/
};

} // namespace nq

#endif /* PARASITICS_H */
