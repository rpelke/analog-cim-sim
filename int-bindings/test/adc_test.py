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
import json
import os
import sys

repo_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))
sys.path.append(repo_path)


class TestADC(unittest.TestCase):

    def test_sym_adc_i_diff_w_diff(self):
        m_matrix = 3
        n_matrix = 5
        mat = np.array([-128, -128, -128, -128, -128, 127, 127, 127, 127, 127, -12, 88, 65, 0, -99],
                       dtype=np.int32)
        vec = np.array([-1, -1, -1, -1, -1], dtype=np.int32)

        correct_result = mat.reshape(m_matrix, n_matrix).dot(vec)

        cfg_file = os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/SYM_ADC_1.json")

        with open(cfg_file, "r") as file:
            cfg = json.load(file)

        cfg["alpha"] = 1.0
        cfg["SPLIT"] = [cfg["W_BIT"]]
        cfg["m_mode"] = "I_DIFF_W_DIFF_1XB"
        cfg["I_BIT"] = 1

        adc_range = 2 * cfg["N"] * (cfg["LRS"] - cfg["HRS"])

        resolution = [2, 3, 4, 5, 6, 7, 8]

        for b in resolution:
            cfg["resolution"] = b

            with open(cfg_file, "w") as file:
                json.dump(cfg, file, indent=4)

            acs_int.set_config(os.path.abspath(cfg_file))
            acs_int.cpy(mat, m_matrix, n_matrix)

            res = np.zeros(m_matrix, dtype=np.int32)
            acs_int.mvm(res, vec, mat, m_matrix, n_matrix)

            delta = adc_range / ((2**b) - 1)
            step_size = (cfg["LRS"] - cfg["HRS"]) / (2**(cfg["W_BIT"] - 1))
            max_error = (delta / 2) / step_size

            for c_idx, c in enumerate(correct_result):
                self.assertTrue(round(c - max_error) <= res[c_idx] <= round(c + max_error))

            # self.assertTrue(round(max_error + correct_result[0]) == res[0])

    def test_sym_adc_i_offs_w_diff(self):
        m_matrix = 3
        n_matrix = 5
        mat = np.array([-128, -128, -128, -128, -128, 127, 127, 127, 127, 127, -12, 88, 65, 0, -99],
                       dtype=np.int32)
        vec = np.array([0, 0, 0, 0, 0], dtype=np.int32)

        correct_result = mat.reshape(m_matrix, n_matrix).dot(vec)

        cfg_file = os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/SYM_ADC_2.json")

        with open(cfg_file, "r") as file:
            cfg = json.load(file)

        cfg["alpha"] = 1.0
        cfg["SPLIT"] = [cfg["W_BIT"]]
        cfg["m_mode"] = "I_OFFS_W_DIFF"
        cfg["I_BIT"] = 1

        adc_range = 2 * cfg["N"] * (cfg["LRS"] - cfg["HRS"])

        resolution = [2, 3, 4, 5, 6, 7, 8]

        for b in resolution:
            cfg["resolution"] = b

            with open(cfg_file, "w") as file:
                json.dump(cfg, file, indent=4)

            acs_int.set_config(os.path.abspath(cfg_file))
            acs_int.cpy(mat, m_matrix, n_matrix)

            res = np.zeros(m_matrix, dtype=np.int32)
            acs_int.mvm(res, vec, mat, m_matrix, n_matrix)

            delta = adc_range / ((2**b) - 1)
            step_size = (cfg["LRS"] - cfg["HRS"]) / (2**(cfg["W_BIT"] - 1))
            max_error = (delta / 2) / step_size

            for c_idx, c in enumerate(correct_result):
                self.assertTrue(round(c - max_error) <= res[c_idx] <= round(c + max_error))

            # self.assertTrue(round(correct_result[0] - max_error) == res[0])

    def test_sym_adc_i_tc_w_diff(self):
        m_matrix = 3
        n_matrix = 5
        mat = np.array([-128, -128, -128, -128, -128, 127, 127, 127, 127, 127, -12, 88, 65, 0, -99],
                       dtype=np.int32)
        vec = np.array([-1, -1, -1, -1, -1], dtype=np.int32)

        correct_result = mat.reshape(m_matrix, n_matrix).dot(vec)

        cfg_file = os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/SYM_ADC_3.json")

        with open(cfg_file, "r") as file:
            cfg = json.load(file)

        cfg["alpha"] = 1.0
        cfg["SPLIT"] = [cfg["W_BIT"]]
        cfg["m_mode"] = "I_TC_W_DIFF"
        cfg["I_BIT"] = 1

        adc_range = 2 * cfg["N"] * (cfg["LRS"] - cfg["HRS"])

        resolution = [2, 3, 4, 5, 6, 7, 8]

        for b in resolution:
            cfg["resolution"] = b

            with open(cfg_file, "w") as file:
                json.dump(cfg, file, indent=4)

            acs_int.set_config(os.path.abspath(cfg_file))
            acs_int.cpy(mat, m_matrix, n_matrix)

            res = np.zeros(m_matrix, dtype=np.int32)
            acs_int.mvm(res, vec, mat, m_matrix, n_matrix)

            delta = adc_range / ((2**b) - 1)
            step_size = (cfg["LRS"] - cfg["HRS"]) / (2**(cfg["W_BIT"] - 1))
            max_error = (delta / 2) / step_size

            for c_idx, c in enumerate(correct_result):
                self.assertTrue(round(c - max_error) <= res[c_idx] <= round(c + max_error))

            # self.assertTrue(round(correct_result[0] + max_error) == res[0])

    def test_sym_adc_i_uint_w_diff(self):
        m_matrix = 3
        n_matrix = 5
        mat = np.array([-128, -128, -128, -128, -128, 127, 127, 127, 127, 127, -12, 88, 65, 0, -99],
                       dtype=np.int32)
        vec = np.array([1, 1, 1, 1, 1], dtype=np.int32)

        correct_result = mat.reshape(m_matrix, n_matrix).dot(vec)

        cfg_file = os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/SYM_ADC_4.json")

        with open(cfg_file, "r") as file:
            cfg = json.load(file)

        cfg["alpha"] = 1.0
        cfg["SPLIT"] = [cfg["W_BIT"]]
        cfg["m_mode"] = "I_UINT_W_DIFF"
        cfg["I_BIT"] = 1

        adc_range = 2 * cfg["N"] * (cfg["LRS"] - cfg["HRS"])

        resolution = [2, 3, 4, 5, 6, 7, 8]

        for b in resolution:
            cfg["resolution"] = b

            with open(cfg_file, "w") as file:
                json.dump(cfg, file, indent=4)

            acs_int.set_config(os.path.abspath(cfg_file))
            acs_int.cpy(mat, m_matrix, n_matrix)

            res = np.zeros(m_matrix, dtype=np.int32)
            acs_int.mvm(res, vec, mat, m_matrix, n_matrix)

            delta = adc_range / ((2**b) - 1)
            step_size = (cfg["LRS"] - cfg["HRS"]) / (2**(cfg["W_BIT"] - 1))
            max_error = (delta / 2) / step_size

            for c_idx, c in enumerate(correct_result):
                self.assertTrue(round(c - max_error) <= res[c_idx] <= round(c + max_error))

            # self.assertTrue(round(correct_result[0] - max_error) == res[0])

    def test_pos_adc_i_uint_w_offs(self):
        m_matrix = 3
        n_matrix = 5
        mat = np.array([127, 127, 127, 127, 127, -128, -128, -128, -128, -128, 0, 0, 0, 0, 0],
                       dtype=np.int32)
        vec = np.array([1, 1, 1, 1, 1], dtype=np.int32)

        correct_result = mat.reshape(m_matrix, n_matrix).dot(vec)

        cfg_file = os.path.abspath(f"{repo_path}/cpp/test/lib/configs/analog/POS_ADC_1.json")

        with open(cfg_file, "r") as file:
            cfg = json.load(file)

        cfg["alpha"] = 1.0
        cfg["SPLIT"] = [cfg["W_BIT"]]
        cfg["m_mode"] = "I_UINT_W_OFFS"
        cfg["I_BIT"] = 1

        adc_range = cfg["N"] * cfg["LRS"]

        resolution = [3, 4, 5, 6, 7, 8]

        for b in resolution:
            cfg["resolution"] = b

            with open(cfg_file, "w") as file:
                json.dump(cfg, file, indent=4)

            acs_int.set_config(os.path.abspath(cfg_file))
            acs_int.cpy(mat, m_matrix, n_matrix)

            res = np.zeros(m_matrix, dtype=np.int32)
            acs_int.mvm(res, vec, mat, m_matrix, n_matrix)

            delta = adc_range / ((2**b) - 1)
            step_size = (cfg["LRS"] - cfg["HRS"]) / (2**(cfg["W_BIT"]) - 1)
            max_error = (delta / 2) / step_size

            for c_idx, c in enumerate(correct_result):
                self.assertTrue(round(c - max_error) <= res[c_idx] <= round(c + max_error))

            # self.assertTrue(res[0] == correct_result[0])


if __name__ == "__main__":
    unittest.main()
