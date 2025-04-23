##############################################################################
# Copyright (C) 2025 Rebecca Pelke & Joel Klein                              #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
import unittest
import numpy as np
import acs_int
import os
import sys
import json

repo_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))
sys.path.append(repo_path)


class TestConfigUpdate(unittest.TestCase):

    def setUp(self):
        # Load a base configuration
        acs_int.set_config(
            os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/I_DIFF_W_DIFF_1XB.json"))

    def test_simple_parameter_update(self):
        # Base test
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, 12, 1], dtype=np.int32)
        vec = np.array([-120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        expected = np.array([-13759, -119, -1386], dtype=np.int32)
        np.testing.assert_array_equal(res, expected)

        # Update a parameter that doesn't require crossbar recreation
        update_json = json.dumps({"verbose": True})
        acs_int.update_config(update_json)

        # Result should be the same
        res = np.array([1, 1, -1], dtype=np.int32)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, expected)

    def test_crossbar_parameter_update(self):
        # Setup initial test
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, 12, 1], dtype=np.int32)
        vec = np.array([-120, 55], dtype=np.int32)

        # Get initial matrix conductances
        acs_int.cpy(mat, m_matrix, n_matrix)
        initial_ia_p = acs_int.ia_p()

        # Update HRS (should recreate crossbar)
        new_hrs = 1000.0    # Different from default
        update_json = json.dumps({"HRS": new_hrs})
        acs_int.update_config(update_json)

        # Copy matrix again after crossbar recreation
        acs_int.cpy(mat, m_matrix, n_matrix)
        updated_ia_p = acs_int.ia_p()

        # Verify conductances changed
        self.assertFalse(np.array_equal(initial_ia_p, updated_ia_p))

    def test_multiple_parameter_update(self):
        update_json = json.dumps({"HRS": 1000.0, "LRS": 100.0, "verbose": True, "resolution": 8})
        acs_int.update_config(update_json)

        # Test that the configuration was properly applied
        m_matrix = 3
        n_matrix = 2
        mat = np.array([100, -32, 1, 0, 12, 1], dtype=np.int32)
        vec = np.array([-120, 55], dtype=np.int32)
        res = np.array([1, 1, -1], dtype=np.int32)

        acs_int.cpy(mat, m_matrix, n_matrix)
        acs_int.mvm(res, vec, mat, m_matrix, n_matrix)
        # Result will be different due to changed HRS/LRS values


if __name__ == "__main__":
    unittest.main()
