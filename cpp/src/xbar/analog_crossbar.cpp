#include "xbar/analog_crossbar.h"

namespace nq {

AnalogCrossbar::AnalogCrossbar(uint32_t M, uint32_t N, uint32_t W_BIT, uint32_t I_BIT,
std::vector<uint32_t> SPLIT, const float HRS, const float LRS, const INT8MappingMode m_mode,
const ADCType adc_type, const float min_adc_curr, const float max_adc_curr, const float alpha, const uint32_t resolution) :
    m_(M),
    n_(N),
    w_bit_(W_BIT),
    i_bit_(I_BIT),
    split_(SPLIT),
    ia_p_(m_ * split_.size(), std::vector<float>(n_, 0)),
    ia_m_(m_ * split_.size(), std::vector<float>(n_, 0)),
    hrs_(HRS),
    lrs_(LRS),
    i_step_size_(split_.size(), 0.0),
    shift_(SPLIT.size(), 0),
    adc_(ADCFactory::createADC(adc_type, min_adc_curr, max_adc_curr, alpha, resolution)),
    m_mode_(m_mode),
    delta_i_uint_w_offs_(0.0) {
    
    num_segments_ = split_.size();
    i_mm_ = lrs_ - hrs_;

    for (size_t s = 0; s < num_segments_; ++s) {
        i_step_size_[s] = i_mm_ / ((1 << split_[s]) - 1);
    }

    int curr_w_bit = w_bit_;
    for (size_t i = 0; i < split_.size(); ++i) {
        shift_[i] = curr_w_bit - split_[i];
        curr_w_bit -= split_[i];
    }

    // Calculate delta factor for I_UINT_W_OFFS mode
    if (m_mode_ == INT8MappingMode::I_UINT_W_OFFS) {
        int curr_w_bit = w_bit_;
        for (size_t i = 0; i < split_.size(); ++i) {
            shift_[i] = curr_w_bit - split_[i];
            curr_w_bit -= split_[i];
            delta_i_uint_w_offs_ += (1<<shift_[i]) * ((1<<split_[i]) - 1);
        }
        delta_i_uint_w_offs_ = hrs_/i_mm_ * delta_i_uint_w_offs_;
    }
}

// Write positive and negative analog cells
void AnalogCrossbar::write_p_m(const std::vector<std::vector<int32_t>>& gd_p, const std::vector<std::vector<int32_t>>& gd_m, int32_t m_matrix, int32_t n_matrix) {
    float hrs = hrs_;
    for (size_t m = 0; m < m_matrix * num_segments_; ++m) {
        float step = i_step_size_[m % num_segments_];
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = gd_p[m][n] * step + hrs;
            ia_m_[m][n] = gd_m[m][n] * step + hrs;
        }
    }
}

// Write positive analog cells only
void AnalogCrossbar::write_p(const std::vector<std::vector<int32_t>>& gd_p, int32_t m_matrix, int32_t n_matrix) {
    float hrs = hrs_;
    for (size_t m = 0; m < m_matrix * num_segments_; ++m) {
        float step = i_step_size_[m % num_segments_];
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = gd_p[m][n] * step + hrs;
        }
    }
}

void AnalogCrossbar::mvm(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const, int32_t inp_sum) {
    if ((m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_1XB) || (m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_2XB)) {
        mvm_i_diff_w_diff(res, vec, mat, m_matrix, n_matrix);
    }
    else if (m_mode_ == INT8MappingMode::I_OFFS_W_DIFF) {
        mvm_i_offs_w_diff(res, vec, mat, m_matrix, n_matrix, ct_const);
    }
    else if (m_mode_ == INT8MappingMode::I_TC_W_DIFF) {
        mvm_i_tc_w_diff(res, vec, mat, m_matrix, n_matrix);
    }
    else if (m_mode_ == INT8MappingMode::I_UINT_W_DIFF) {
        mvm_i_uint_w_diff(res, vec, mat, m_matrix, n_matrix);
    }
    else if (m_mode_ == INT8MappingMode::I_UINT_W_OFFS) {
        mvm_i_uint_w_offs(res, vec, mat, m_matrix, n_matrix, inp_sum);
    }
    else {
        throw std::runtime_error("Digital-to-analog mapping not implemented.");
    }
}

const std::vector<std::vector<float>>& AnalogCrossbar::get_ga_p() const {
    return ia_p_;
}

const std::vector<std::vector<float>>& AnalogCrossbar::get_ga_m() const {
    return ia_m_;
}

void AnalogCrossbar::mvm_i_diff_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: ia+ (ia_p_) and ia- (ia_m_).
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<float> tmp_out(tmp_size, 0);
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

    // For each bit in vd_p execute one MVM operation with ia_p_ and one with ia_m_
    // For positive inputs vd_p: bit 7 is always 0 (sign bit) -> i_bit_ - 1
    // Subract both results in the analog domain
    for (size_t i_bit = 0; i_bit < i_bit_ - 1; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] += (ia_p_[t_m][n] - ia_m_[t_m][n]) * ((vd_p[n] >> i_bit) & 1);
            }
        }
        
        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split_.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round( // Rounding required for shift operation
                        adc_->analog_digital_conversion( // ADC conversion
                            tmp_out[m * split_.size() + s] / i_step_size_[s]
                        )
                    )
                ) << shift_[s] << i_bit;
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }

    // For each bit in vd_m execute one MVM operation with ia_p_ and one with ia_m_
    // Subract both results in the analog domain
    for (size_t i_bit = 0; i_bit < i_bit_; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] += (ia_m_[t_m][n] - ia_p_[t_m][n]) * ((vd_m[n] >> i_bit) & 1);
            }
        }

        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split_.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round( // Rounding required for shift operation
                        adc_->analog_digital_conversion( // ADC conversion
                            tmp_out[m * split_.size() + s] / i_step_size_[s]
                        )
                    )
                ) << shift_[s] << i_bit;
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }
}

void AnalogCrossbar::mvm_i_offs_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: ia+ (ia_p_) and ia- (ia_m_).
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<float> tmp_out(tmp_size, 0);
    std::vector<int32_t> vd_p(n_matrix, 0);

    // Shift input bits to positive range (+ 2^(B-1))
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p[n] = (1 << (i_bit_ - 1)) + vec[n];
    }

    // For each bit in vd_p execute one MVM operation with ia_p_ and one with ia_m_
    // MSB of input has position: i_bit_ + 1
    // Subract both results in the analog domain
    for (size_t i_bit = 0; i_bit < i_bit_ + 1; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] += (ia_p_[t_m][n] - ia_m_[t_m][n]) * ((vd_p[n] >> i_bit) & 1);
            }
        }
        
        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split_.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round( // Rounding required for shift operation
                        adc_->analog_digital_conversion( // ADC conversion
                            tmp_out[m * split_.size() + s] / i_step_size_[s]
                        )
                    )
                ) << shift_[s] << i_bit;
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }

    // Subtract term of compile-time constant
    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] -= ((*ct_const)[m] << (i_bit_ - 1));
    }
}

void AnalogCrossbar::mvm_i_tc_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: ia+ (ia_p_) and ia- (ia_m_).
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<float> tmp_out(tmp_size, 0);

    // For each bit in vec execute one MVM operation with ia_p_ and one with ia_m_
    // Execute all multiplications with all positive interpreted inputs first.
    for (size_t i_bit = 0; i_bit < i_bit_ - 1; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] += (ia_p_[t_m][n] - ia_m_[t_m][n]) * ((vec[n] >> i_bit) & 1);
            }
        }
        
        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split_.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round( // Rounding required for shift operation
                        adc_->analog_digital_conversion( // ADC conversion
                            tmp_out[m * split_.size() + s] / i_step_size_[s]
                        )
                    )
                ) << shift_[s] << i_bit;
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }

    // Execute "negative MVM" for the sign bit of vec at pos i_bit_ - 1
    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (ia_p_[t_m][n] - ia_m_[t_m][n]) * ((vec[n] >> (i_bit_ - 1)) & 1);
        }
    }
    // Addition of the partial results caused by splitted weights
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split_.size(); ++s) {
            res[m] -= static_cast<int32_t>( // Subtract
                round( // Rounding required for shift operation
                    adc_->analog_digital_conversion( // ADC conversion
                        tmp_out[m * split_.size() + s] / i_step_size_[s]
                    )
                )
            ) << shift_[s] << (i_bit_ - 1);
        }
    }
}

void AnalogCrossbar::mvm_i_uint_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Two matrices exist: ia+ (ia_p_) and ia- (ia_m_)
    // The input is already positive only
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<float> tmp_out(tmp_size, 0);

    // For each bit in vec execute one MVM operation with ia_p_ and one with ia_m_
    // Execute all multiplications with all positive interpreted inputs first.
    for (size_t i_bit = 0; i_bit < i_bit_; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] += (ia_p_[t_m][n] - ia_m_[t_m][n]) * ((vec[n] >> i_bit) & 1);
            }
        }
        
        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split_.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round( // Rounding required for shift operation
                        adc_->analog_digital_conversion( // ADC conversion
                            tmp_out[m * split_.size() + s] / i_step_size_[s]
                        )
                    )
                ) << shift_[s] << i_bit;
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }
}

void AnalogCrossbar::mvm_i_uint_w_offs(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, int32_t inp_sum) {
    // The splitted matrix is of size split_size*M x N (split_size values per original matrix value)
    // Only one matrix exist: ia+ (ia_p_)
    // The input is already positive only
    const uint32_t tmp_size = m_matrix * split_.size();
    std::vector<float> tmp_out(tmp_size, 0);
    std::vector<float> res_fp(m_matrix, 0);

    // For each bit in vec execute one MVM operation with ia_p_
    for (size_t i_bit = 0; i_bit < i_bit_; ++i_bit) {
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] += ia_p_[t_m][n] * ((vec[n] >> i_bit) & 1);
            }
        }
        
        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split_.size(); ++s) {
                // No rounding is done here, so multiply instead of shift
                // tmp_out / i_step_size_[s] is a floating-point value
                res_fp[m] += adc_->analog_digital_conversion(
                    (tmp_out[m * split_.size() + s] / i_step_size_[s]) 
                ) * (1 << shift_[s]) * (1 << i_bit);
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }

    // Rescaling of the results and rounding
    for (size_t m = 0; m < m_matrix; ++m) {
        res_fp[m] -= inp_sum * (delta_i_uint_w_offs_ + (1 << (w_bit_ - 1)));
        res[m] += static_cast<int32_t>(round(res_fp[m]));
    }
}

} // end namespace
