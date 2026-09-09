##############################################################################
# Copyright (C) 2026 Joel Klein                                              #
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
import json

from helper.native_output import suppressed_native_output

repo_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))
sys.path.append(repo_path)


class TestSizeCheck(unittest.TestCase):

    def setUp(self):
        # BNN_I is differential, so one weight takes two columns and a 4x4
        # crossbar holds a 2x4 logical matrix.
        acs_py.set_config(os.path.abspath(f"{repo_path}/cpp/test/lib/configs/digital/BNN_I.json"))
        acs_py.update_config(json.dumps({"M": 4, "N": 4}))

    def test_matrix_at_capacity_is_accepted(self):
        m_matrix = 2
        n_matrix = 4
        mat = np.ones(m_matrix * n_matrix, dtype=np.int32)
        vec = np.ones(n_matrix, dtype=np.int32)
        res = np.zeros(m_matrix, dtype=np.int32)

        self.assertEqual(acs_py.cpy(mat, m_matrix, n_matrix), 0)
        self.assertEqual(acs_py.mvm(res, vec, mat, m_matrix, n_matrix), 0)

    def test_oversized_matrix_is_rejected(self):
        # One logical column more than the crossbar can map.
        m_matrix = 3
        n_matrix = 4
        mat = np.ones(m_matrix * n_matrix, dtype=np.int32)
        vec = np.ones(n_matrix, dtype=np.int32)
        res = np.zeros(m_matrix, dtype=np.int32)

        with suppressed_native_output(stderr=True):
            self.assertEqual(acs_py.cpy(mat, m_matrix, n_matrix), -1)
            self.assertEqual(acs_py.mvm(res, vec, mat, m_matrix, n_matrix), -1)

    def test_rejected_mvm_leaves_result_untouched(self):
        m_matrix = 3
        n_matrix = 4
        mat = np.ones(m_matrix * n_matrix, dtype=np.int32)
        vec = np.ones(n_matrix, dtype=np.int32)
        res = np.zeros(m_matrix, dtype=np.int32)

        with suppressed_native_output(stderr=True):
            acs_py.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.zeros(m_matrix, dtype=np.int32))


if __name__ == "__main__":
    unittest.main()
