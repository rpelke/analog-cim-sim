import ctypes


def exe_mvm(lib, res, vec, mat, m_matrix, n_matrix):
    lib.exe_mvm(res.ctypes.data_as(ctypes.POINTER(ctypes.c_int32)),
                vec.ctypes.data_as(ctypes.POINTER(ctypes.c_int32)),
                mat.ctypes.data_as(ctypes.POINTER(ctypes.c_int32)),
                m_matrix, n_matrix)

def cpy_mtrx(lib, mat, m_matrix, n_matrix):
    lib.cpy_mtrx(mat.ctypes.data_as(ctypes.POINTER(ctypes.c_int32)),
                 m_matrix, n_matrix)


def load_mvm_lib(file: str = None, set_callback: bool = True) -> ctypes.CFUNCTYPE :
    assert file != None, "Path to library file not specified."
    lib = ctypes.CDLL(file, ctypes.RTLD_GLOBAL)

    lib.exe_mvm.argtypes = [
        ctypes.POINTER(ctypes.c_int32),  # res
        ctypes.POINTER(ctypes.c_int32),  # vec
        ctypes.POINTER(ctypes.c_int32),  # mat
        ctypes.c_int32,                  # m_matrix
        ctypes.c_int32                   # n_matrix
    ]
    lib.exe_mvm.restype = ctypes.c_int32

    lib.cpy_mtrx.argtypes = [
        ctypes.POINTER(ctypes.c_int32),  # mat
        ctypes.c_int32,                  # m_matrix
        ctypes.c_int32                   # n_matrix
    ]
    lib.cpy_mtrx.restype = ctypes.c_int32

    return lib
