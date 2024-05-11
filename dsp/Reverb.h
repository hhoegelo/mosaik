#pragma once

#include <array>
#include <vector>
#include <cstdint>

namespace Dsp
{

  class Reverb
  {

    using DelayOffset = std::array<unsigned, 9>;

    enum OffsetIndex : unsigned
    {
      Offset_1,
      Offset_2,
      Offset_3,
      Offset_4,
      Offset_5,
      Offset_6,
      Offset_7,
      Offset_8,
      Offset_9
    };

    // primes
    static constexpr DelayOffset s_offset_left = { 281, 1123, 863, 467, 719, 1031, 887, 1217, 2917 };
    static constexpr DelayOffset s_offset_right = { 379, 1103, 929, 491, 683, 1019, 859, 1367, 2677 };

    DelayOffset m_offset_left = {};
    DelayOffset m_offset_right = {};

   public:
    Reverb();

    void set(float size, float chorus, float bal, float pre, float color);
    void apply(float _rawSample_L, float _rawSample_R, float send, float feedback, float dry, float wet);
    void resetDSP();

    float m_out_L = 0.0f, m_out_R = 0.0f, m_out_dry = 0.0f, m_out_wet = 0.0f;

   private:
    uint32_t m_slow_tick = 0;
    uint32_t m_slow_thrsh = 0;

    //************************** Reverb Modulation ***************************//
    float m_mod_1a = 0.0f, m_mod_2a = 0.0f, m_mod_1b = 0.0f, m_mod_2b = 0.0f;
    float m_lfo_omega_1 = 0.0f, m_lfo_omega_2 = 0.0f;
    float m_lfo_stateVar_1 = 0.0f, m_lfo_stateVar_2 = 0.0f;

    //****************************** Loop Filter *****************************//
    float m_warpConst_PI = 0.0f;
    float m_omegaClip_max = 0.0f;

    float m_lp_a0 = 0.0f, m_lp_a1 = 0.0f, m_lp_omega = 0.0f;
    float m_hp_a0 = 0.0f, m_hp_a1 = 0.0f, m_hp_omega = 0.0f;

    float m_lp_stateVar_L = 0.0f, m_lp_stateVar_R = 0.0f;
    float m_hp_stateVar_L = 0.0f, m_hp_stateVar_R = 0.0f;

    //***************************** Delay Buffer *****************************//
    float m_fb_amnt = 0.0f;
    float m_absorb = 0.0f;
    float m_bal_half = 0.0f, m_bal_full = 0.0f;

    uint32_t m_buffer_indx = 0;
    uint32_t m_buffer_sz_m1 = 0, m_buffer_sz_m2 = 0;

    float m_preDel_L = 0.0f, m_preDel_R = 0.0f;

    std::vector<float> m_buffer_L;
    std::vector<float> m_buffer_R;

    std::vector<float> m_buffer_L1;
    std::vector<float> m_buffer_L2;
    std::vector<float> m_buffer_L3;
    std::vector<float> m_buffer_L4;
    std::vector<float> m_buffer_L5;
    std::vector<float> m_buffer_L6;
    std::vector<float> m_buffer_L7;
    std::vector<float> m_buffer_L8;
    std::vector<float> m_buffer_L9;

    std::vector<float> m_buffer_R1;
    std::vector<float> m_buffer_R2;
    std::vector<float> m_buffer_R3;
    std::vector<float> m_buffer_R4;
    std::vector<float> m_buffer_R5;
    std::vector<float> m_buffer_R6;
    std::vector<float> m_buffer_R7;
    std::vector<float> m_buffer_R8;
    std::vector<float> m_buffer_R9;

    float m_stateVar_L1 = 0.0f, m_stateVar_R1 = 0.0f;
    float m_stateVar_L2 = 0.0f, m_stateVar_R2 = 0.0f;
    float m_stateVar_L3 = 0.0f, m_stateVar_R3 = 0.0f;
    float m_stateVar_L4 = 0.0f, m_stateVar_R4 = 0.0f;
    float m_stateVar_L5 = 0.0f, m_stateVar_R5 = 0.0f;
    float m_stateVar_L6 = 0.0f, m_stateVar_R6 = 0.0f;
    float m_stateVar_L7 = 0.0f, m_stateVar_R7 = 0.0f;
    float m_stateVar_L8 = 0.0f, m_stateVar_R8 = 0.0f;
    float m_stateVar_L9 = 0.0f, m_stateVar_R9 = 0.0f;

    //******************************* Smoothing ******************************//
    float m_smooth_inc = 0.0f;

    float m_depth = 0.0f;
    float m_depth_ramp = 0.0f;
    float m_depth_target = 0.0f;
    float m_depth_base = 0.0f;
    float m_depth_diff = 0.0f;

    float m_size = 0.0f;
    float m_size_ramp = 0.0f;
    float m_size_target = 0.0f;
    float m_size_base = 0.0f;
    float m_size_diff = 0.0f;

    float m_bal = 0.0f;
    float m_bal_ramp = 0.0f;
    float m_bal_target = 0.0f;
    float m_bal_base = 0.0f;
    float m_bal_diff = 0.0f;

    float m_preDel_L_ramp = 0.0f;
    float m_preDel_L_target = 0.0f;
    float m_preDel_L_base = 0.0f;
    float m_preDel_L_diff = 0.0f;

    float m_preDel_R_ramp = 0.0f;
    float m_preDel_R_target = 0.0f;
    float m_preDel_R_base = 0.0f;
    float m_preDel_R_diff = 0.0f;

    float m_lp_omega_ramp = 0.0f;
    float m_lp_omega_target = 0.0f;
    float m_lp_omega_base = 0.0f;
    float m_lp_omega_diff = 0.0f;

    float m_hp_omega_ramp = 0.0f;
    float m_hp_omega_target = 0.0f;
    float m_hp_omega_base = 0.0f;
    float m_hp_omega_diff = 0.0f;
  };
}