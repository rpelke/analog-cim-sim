##############################################################################
# Copyright (C) 2025 Rebecca Pelke                                           #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
import unittest
import numpy as np
import acs_py
import os
import sys

from helper.native_output import suppressed_native_output
from helper.rd_model import ReadDisturbGoldenModel

repo_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))
sys.path.append(repo_path)


class TestReadDisturbModel(unittest.TestCase):

    def test_lrs_change(self):
        rd_gm = ReadDisturbGoldenModel(-0.4, 100e-9)
        I_HRS = 5.0
        I_LRS = 30.0
        G_LRS = I_LRS / rd_gm.V_read

        m_matrix = 1
        n_matrix = 1
        mat = np.array([-1], dtype=np.int32)
        vec = np.array([1], dtype=np.int32)
        res = np.array([0], dtype=np.int32)

        acs_py.set_config(
            os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/READ_DISTURB.json"))
        assert acs_py.write_ops() == 0
        assert acs_py.mvm_ops() == 0
        assert acs_py.read_ops() == 0

        total_mvm_ops = 0

        N_cycles = [1, 100, 1000]
        mvms_to_execute = [1] + [N_cycles[i] - N_cycles[i - 1] for i in range(1, len(N_cycles))]

        t = [1e-4, 1e-2, 2e-2]
        t_to_stress = [t[0]] + [t[i] - t[i - 1] for i in range(1, len(t))]

        for N_idx, N in enumerate(N_cycles):
            for n in range(mvms_to_execute[N_idx]):
                mat[0] = +1
                acs_py.cpy(mat, m_matrix, n_matrix)
                mat[0] = -1
                acs_py.cpy(mat, m_matrix, n_matrix)

            assert acs_py.write_ops() == 2 * N
            assert acs_py.read_ops() == 0
            assert acs_py.ia_p()[0][0] == I_HRS
            assert acs_py.ia_m()[0][0] == I_LRS

            for t_idx, stress_time in enumerate(t):
                num_reads = int(round(t_to_stress[t_idx] / rd_gm.t_read))

                for r in range(num_reads):
                    acs_py.mvm(res, vec, mat, m_matrix, n_matrix)
                    total_mvm_ops += 1
                assert acs_py.mvm_ops() == total_mvm_ops

                ia_p = acs_py.ia_p()[0][0]
                ia_m = acs_py.ia_m()[0][0]

                G_LRS_new = G_LRS * rd_gm.G_scaling(stress_time, N - 1)
                I_LRS_new = G_LRS_new * rd_gm.V_read

                # No read disturb mitigation
                assert acs_py.refresh_cell_ops() == 0
                assert acs_py.refresh_ops() == 0

                assert acs_py.cycles_p()[0][0] == N
                assert acs_py.cycles_m()[0][0] == N - 1

                np.testing.assert_allclose(ia_m, I_LRS_new, atol=1e-6)
                np.testing.assert_allclose(ia_p, I_HRS, atol=0.0)

    def test_rd_model_boundaries(self):
        rd_gm = ReadDisturbGoldenModel(-0.4, 100e-9)
        I_HRS = 5.0
        I_LRS = 30.0

        m_matrix = 1
        n_matrix = 1
        mat = np.array([-1], dtype=np.int32)
        vec = np.array([1], dtype=np.int32)
        res = np.array([0], dtype=np.int32)

        acs_py.set_config(
            os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/READ_DISTURB.json"))
        assert acs_py.write_ops() == 0
        assert acs_py.mvm_ops() == 0
        assert acs_py.read_ops() == 0

        N_cycles = int(1.4e6)
        for n in range(N_cycles):
            mat[0] = +1
            acs_py.cpy(mat, m_matrix, n_matrix)
            mat[0] = -1
            acs_py.cpy(mat, m_matrix, n_matrix)

        assert acs_py.write_ops() == 2 * N_cycles
        assert acs_py.read_ops() == 0
        assert acs_py.ia_p()[0][0] == I_HRS
        assert acs_py.ia_m()[0][0] == I_LRS
        assert acs_py.rd_run_out_of_bounds() == False

        acs_py.mvm(res, vec, mat, m_matrix, n_matrix)

        N_cycles = int(0.1e6)
        for n in range(N_cycles):
            mat[0] = +1
            acs_py.cpy(mat, m_matrix, n_matrix)
            mat[0] = -1
            acs_py.cpy(mat, m_matrix, n_matrix)

        # Leaving the model range is the point of the test, so the warning the
        # read belongs to is expected here.
        with suppressed_native_output(stderr=True):
            acs_py.mvm(res, vec, mat, m_matrix, n_matrix)
        assert acs_py.rd_run_out_of_bounds() == True

    def test_negative_matrix_is_disturbed(self):
        # TNN_IV keeps the MSB in the negative matrix, so those cells age like the
        # ones in the positive matrix.
        rd_gm = ReadDisturbGoldenModel(-0.4, 100e-9)
        I_LRS = 30.0

        m_matrix = 1
        n_matrix = 1
        mat = np.array([0], dtype=np.int32)
        vec = np.array([1], dtype=np.int32)
        res = np.array([0], dtype=np.int32)

        acs_py.set_config(
            os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/READ_DISTURB_TNN_IV.json"))

        # -1 sets both bits, 0 clears them again, which counts one set-reset
        # cycle for both cells
        for value in [-1, 0, -1]:
            mat[0] = value
            acs_py.cpy(mat, m_matrix, n_matrix)

        assert acs_py.cycles_p()[0][0] == 1
        assert acs_py.cycles_m()[0][0] == 1
        assert acs_py.ia_p()[0][0] == I_LRS
        assert acs_py.ia_m()[0][0] == I_LRS

        for r in range(int(round(1e-2 / rd_gm.t_read))):
            acs_py.mvm(res, vec, mat, m_matrix, n_matrix)

        ia_p = acs_py.ia_p()[0][0]
        ia_m = acs_py.ia_m()[0][0]

        assert ia_p < I_LRS
        # Both cells see the same stress, so they end up at the same current
        np.testing.assert_allclose(ia_m, ia_p, atol=0.0)


if __name__ == "__main__":
    unittest.main()
