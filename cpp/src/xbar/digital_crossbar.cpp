#include "xbar/digital_crossbar.h"

namespace nq {

DigitalCrossbar::DigitalCrossbar(uint32_t M, uint32_t N, uint32_t W_BIT, uint32_t I_BIT, std::vector<uint32_t> SPLIT, const bool digital_only, const INT8MappingMode m_mode) :
    m_(M),
    n_(N),
    w_bit_(W_BIT),
    i_bit_(I_BIT),
    split_(SPLIT),
    gd_p_(m_ * split_.size(), std::vector<int32_t>(n_, 0)),
    gd_m_(m_ * split_.size(), std::vector<int32_t>(n_, 0)),
    shift_(SPLIT.size(), 0),
    digital_only_(digital_only),
    m_mode_(m_mode),
    ct_constants_(M, 0) {
    
    int curr_w_bit = w_bit_;
    for (size_t i = 0; i < split_.size(); ++i) {
        shift_[i] = curr_w_bit - split_[i];
        curr_w_bit -= split_[i];
    }
}

void DigitalCrossbar::write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    if ((m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_1XB)
    || (m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_2XB)
    || (m_mode_ == INT8MappingMode::I_TC_W_DIFF)
    || (m_mode_ == INT8MappingMode::I_UINT_W_DIFF))
    {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                int mat_val = mat[n_matrix * m + n];
                for (size_t s = 0; s < split_.size(); ++s) {
                    int gd_idx = m * split_.size() + s;
                    if (mat_val >= 0) {
                        gd_p_[gd_idx][n] = (mat_val >> shift_[s]) & ((1 << split_[s]) - 1);
                        gd_m_[gd_idx][n] = 0;
                    }
                    else {
                        gd_p_[gd_idx][n] = 0;
                        gd_m_[gd_idx][n] = (-mat_val >> shift_[s]) & ((1 << split_[s]) - 1);
                    }
                }
            }
        }
    }
    else if (m_mode_ == INT8MappingMode::I_OFFS_W_DIFF) {
        for (size_t m = 0; m < m_matrix; ++m) {
            int32_t sum_n = 0;
            for (size_t n = 0; n < n_matrix; ++n) {
                int mat_val = mat[n_matrix * m + n];
                sum_n += mat_val;
                for (size_t s = 0; s < split_.size(); ++s) {
                    int gd_idx = m * split_.size() + s;
                    if (mat_val >= 0) {
                        gd_p_[gd_idx][n] = (mat_val >> shift_[s]) & ((1 << split_[s]) - 1);
                        gd_m_[gd_idx][n] = 0;
                    }
                    else {
                        gd_p_[gd_idx][n] = 0;
                        gd_m_[gd_idx][n] = (-mat_val >> shift_[s]) & ((1 << split_[s]) - 1);
                    }
                }
            }
            ct_constants_[m] = sum_n;
        }
    }
    else if (m_mode_ == INT8MappingMode::I_UINT_W_OFFS) {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                int mat_val = mat[n_matrix * m + n] + (1 << (w_bit_ - 1));
                for (size_t s = 0; s < split_.size(); ++s) {
                    int gd_idx = m * split_.size() + s;
                    gd_p_[gd_idx][n] = (mat_val >> shift_[s]) & ((1 << split_[s]) - 1);
                }
            }
        }
    }
    else {
        throw std::runtime_error("Digital-to-analog mapping not implemented.");
    }
}

void DigitalCrossbar::mvm(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const) {
    if ((m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_1XB) || (m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_2XB)) {
        mvm_i_diff_w_diff(res, vec, mat, m_matrix, n_matrix);
    }
    else if (m_mode_ == INT8MappingMode::I_OFFS_W_DIFF) {
        mvm_i_offs_w_diff(res, vec, mat, m_matrix, n_matrix, &ct_constants_);
    }
    else if (m_mode_ == INT8MappingMode::I_TC_W_DIFF) {
        mvm_i_tc_w_diff(res, vec, mat, m_matrix, n_matrix);
    }
    else if (m_mode_ == INT8MappingMode::I_UINT_W_DIFF) {
        mvm_i_uint_w_diff(res, vec, mat, m_matrix, n_matrix);
    }
    else if (m_mode_ == INT8MappingMode::I_UINT_W_OFFS) {
        mvm_i_uint_w_offs(res, vec, mat, m_matrix, n_matrix);
    }
    else {
        throw std::runtime_error("Digital-to-analog mapping not implemented.");
    }
}

const std::vector<std::vector<int32_t>>& DigitalCrossbar::get_gd_p() const {
    return gd_p_;
}

const std::vector<std::vector<int32_t>>& DigitalCrossbar::get_gd_m() const {
    return gd_m_;
}

int DigitalCrossbar::get_m_xbar() const {
    return gd_p_.size(); 
}

int DigitalCrossbar::get_n_xbar() const {
    return gd_p_[0].size(); 
}

const std::vector<int32_t>& DigitalCrossbar::get_ct_constants() const {
    return ct_constants_;
}

void DigitalCrossbar::mvm_i_diff_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: gd+ (gd_p_) and gd- (gd_m_)
    // The input is also split into positive and negative values
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<int32_t> tmp_out(tmp_size, 0);
    std::vector<int32_t> vd_p(n_matrix, 0);
    std::vector<int32_t> vd_m(n_matrix, 0);

    for (size_t n = 0; n < n_matrix; ++n) {
        if (vec[n] >= 0) {
            vd_p[n] = vec[n];
            vd_m[n] = 0;
        }
        else {
            vd_p[n] = 0;
            vd_m[n] = -vec[n];
        }
    }

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (gd_p_[t_m][n] - gd_m_[t_m][n]) * vd_p[n];
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split_.size(); ++s) {
            res[m] += tmp_out[m * split_.size() + s] << shift_[s];
        }
    }

    std::fill(tmp_out.begin(), tmp_out.end(), 0);

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (gd_m_[t_m][n] - gd_p_[t_m][n]) * vd_m[n];
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split_.size(); ++s) {
            res[m] += tmp_out[m * split_.size() + s] << shift_[s];
        }
    }
}

void DigitalCrossbar::mvm_i_offs_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: gd+ (gd_p_) and gd- (gd_m_)
    // The input (which is signed) is shifted to the positive domain
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<int32_t> tmp_out(tmp_size, 0);
    std::vector<int32_t> vd_p(n_matrix, 0);

    // Shift input bits to positive range (+ 2^(B-1))
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p[n] = (1 << (i_bit_ - 1)) + vec[n];
    }

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (gd_p_[t_m][n] - gd_m_[t_m][n]) * vd_p[n];
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split_.size(); ++s) {
            res[m] += tmp_out[m * split_.size() + s] << shift_[s];
        }
        // Subtract term of compile-time constant
        res[m] -= ((*ct_const)[m] << (i_bit_ - 1));
    }
}

void DigitalCrossbar::mvm_i_tc_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: gd+ (gd_p_) and gd- (gd_m_)
    // In this case, the input values (which are signed) are interpreted as two's complement
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<int32_t> tmp_out(tmp_size, 0);
    std::vector<int32_t> vd_p(n_matrix, 0);

    // Use positive part only
    uint32_t mask = (1 << (i_bit_ - 1)) - 1; // mask(8) = 0b01111111
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p[n] = mask & vec[n];
    }
    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (gd_p_[t_m][n] - gd_m_[t_m][n]) * vd_p[n];
        }
    }

    // Use negative part (MSB of input)
    mask = 1 << (i_bit_ - 1); // mask(8) = 0b10000000
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p[n] = mask & vec[n];
    }
    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] -= (gd_p_[t_m][n] - gd_m_[t_m][n]) * vd_p[n];
        }
    }

    // Add sums caused by splitted weights
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split_.size(); ++s) {
            res[m] += tmp_out[m * split_.size() + s] << shift_[s];
        }
    }
}

void DigitalCrossbar::mvm_i_uint_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: gd+ (gd_p_) and gd- (gd_m_)
    // In this case, the input values 'vec' are stored in int32_t but they are all positive
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<int32_t> tmp_out(tmp_size, 0);

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (gd_p_[t_m][n] - gd_m_[t_m][n]) * vec[n];
        }
    }

    // Add sums caused by splitted weights
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split_.size(); ++s) {
            res[m] += tmp_out[m * split_.size() + s] << shift_[s];
        }
    }
}

void DigitalCrossbar::mvm_i_uint_w_offs(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Only one matrix exist: gd+ (gd_p_)
    // The input values 'vec' are stored in int32_t and they are all positive
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<int32_t> tmp_out(tmp_size, 0);
    int64_t inp_sum = 0;

    // Calculate sum over all inputs
    for (size_t n = 0; n < n_matrix; ++n) {
        inp_sum += vec[n];
    }

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += gd_p_[t_m][n] * vec[n];
        }
    }

    // Add sums caused by splitted weights
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split_.size(); ++s) {
            res[m] += tmp_out[m * split_.size() + s] << shift_[s];
        }
        // Subtract term of compile-time constant
        res[m] -= inp_sum << (w_bit_ - 1);
    }

}

} // end namespace
