import unittest
import numpy as np
from acs_int.interface import load_mvm_lib, exe_mvm, cpy_mtrx


class TestIntInterface(unittest.TestCase):
    def test_simple_mvm(self):
        lib = load_mvm_lib(file="libINT_Interface.so")
        
        m_matrix = 2
        n_matrix = 3
        mat = np.array([1, 2, 3, 4, 5, 6], dtype=np.int32)
        vec = np.array([1, 1, 1], dtype=np.int32)
        res = np.array([0, 0], dtype=np.int32)
        
        cpy_mtrx(lib, mat, m_matrix, n_matrix)
        exe_mvm(lib, res, vec, mat, m_matrix, n_matrix)
        np.testing.assert_array_equal(res, np.array([6, 15], dtype=np.int32))


if __name__ == "__main__":
    unittest.main()
