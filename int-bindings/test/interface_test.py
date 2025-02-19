##############################################################################
# Copyright (C) 2025 Rebecca Pelke                                           #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
import unittest
import numpy as np
import acs_int
import os


class TestIntInterface(unittest.TestCase):
    def test_simple_I_DIFF_W_DIFF_1XB(self):
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, 12, 1], dtype=np.int32)
        vec = np.array([-120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.set_config(os.path.abspath("int-bindings/test/configs/I_DIFF_W_DIFF_1XB.json"))
        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.array([-13759, -119, -1386], dtype=np.int32))

    def test_simple_I_DIFF_W_DIFF_2XB(self):
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, 12, 1], dtype=np.int32)
        vec = np.array([-120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.set_config(os.path.abspath("int-bindings/test/configs/I_DIFF_W_DIFF_2XB.json"))
        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.array([-13759, -119, -1386], dtype=np.int32))

    def test_simple_I_OFFS_W_DIFF(self):
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, 12, 1], dtype=np.int32)
        vec = np.array([-120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.set_config(os.path.abspath("int-bindings/test/configs/I_OFFS_W_DIFF.json"))
        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.array([-13759, -119, -1386], dtype=np.int32))

    def test_simple_I_TC_W_DIFF(self):
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, 12, 1], dtype=np.int32)
        vec = np.array([-120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.set_config(os.path.abspath("int-bindings/test/configs/I_TC_W_DIFF.json"))
        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.array([-13759, -119, -1386], dtype=np.int32))

    def test_simple_I_UINT_W_DIFF(self):
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, -12, 1], dtype=np.int32)
        vec = np.array([120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.set_config(os.path.abspath("int-bindings/test/configs/I_UINT_W_DIFF.json"))
        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.array([10241, 121, -1386], dtype=np.int32))

    def test_simple_I_UINT_W_OFFS(self):
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, -12, 1], dtype=np.int32)
        vec = np.array([120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.set_config(os.path.abspath("int-bindings/test/configs/I_UINT_W_OFFS.json"))
        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.array([10241, 121, -1386], dtype=np.int32))


if __name__ == "__main__":
    unittest.main()
