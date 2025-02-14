#ifndef ANALOG_CROSSBAR_H
#define ANALOG_CROSSBAR_H

#include <cstdint> 
#include <vector>
#include <iostream>
#include <cmath>
#include <memory>

#include "adc/adcfactory.h"
#include "adc/adc.h"
#include "helper/definitions.h"

namespace nq {

class AnalogCrossbar {
    public :
        explicit AnalogCrossbar(uint32_t M, uint32_t N, uint32_t W_BIT, uint32_t I_BIT, std::vector<uint32_t> SPLIT, const float HRS, const float LRS, const INT8MappingMode m_mode,
        const ADCType adc_type, const float min_adc_curr, const float max_adc_curr, const float alpha, const uint32_t resolution);
        AnalogCrossbar() = delete;
        AnalogCrossbar(const AnalogCrossbar&) = delete;
        virtual ~AnalogCrossbar() = default;

        void write_p_m(const std::vector<std::vector<int32_t>>& gd_p, const std::vector<std::vector<int32_t>>& gd_m, int32_t m_matrix, int32_t n_matrix);
        void write_p(const std::vector<std::vector<int32_t>>& gd_p, int32_t m_matrix, int32_t n_matrix);
        void mvm(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const, int32_t inp_sum = 0);
        const std::vector<std::vector<float>>& get_ga_p() const;
        const std::vector<std::vector<float>>& get_ga_m() const;

    private :
        void mvm_i_diff_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
        void mvm_i_offs_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const);
        void mvm_i_tc_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
        void mvm_i_uint_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
        void mvm_i_uint_w_offs(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, int32_t inp_sum);

        const uint32_t m_;
        const uint32_t n_;
        const uint32_t w_bit_;
        const uint32_t i_bit_;
        const std::vector<uint32_t> split_;
        std::vector<std::vector<float>> ia_p_;
        std::vector<std::vector<float>> ia_m_;
        const float hrs_;
        const float lrs_;
        std::vector<float> i_step_size_;
        int num_segments_;
        float i_mm_;
        std::vector<uint32_t> shift_;
        const std::unique_ptr<ADC> adc_;
        const INT8MappingMode m_mode_;
        float delta_i_uint_w_offs_;
};

} // end namespace

#endif
