#include "Reverb.h"
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <cstdio>

namespace Dsp
{
  constexpr auto REVERB_BUFFER_SIZE = 16384;
  constexpr auto REV_G_1 = 0.617748f;
  constexpr auto REV_G_2 = 0.630809f;
  constexpr auto REV_G_3 = 0.64093f;
  constexpr auto REV_G_4 = 0.653011f;

  struct Curve
  {
    float m_Factor1, m_Factor2;
    float m_Startpoint, m_Breakpoint, m_Endpoint;
    float m_Split = 1.f / 2.f;

    Curve(float _startpoint, float _breakpoint, float _endpoint)
    {
      m_Factor1 = (_breakpoint - _startpoint) / (m_Split - 0.f);
      m_Factor2 = (_endpoint - _breakpoint) / (1.f - m_Split);

      m_Startpoint = _startpoint;
      m_Breakpoint = _breakpoint;
      m_Endpoint = _endpoint;
    }

    float applyCurve(float _in)
    {
      float out;

      if(_in <= m_Split)
      {
        out = (_in * m_Factor1) + m_Startpoint;
      }
      else
      {
        out = ((_in - m_Split) * m_Factor2) + m_Breakpoint;
      }

      return out;
    }
  };

  Curve c_reverb_color_curve_1(66.0f, 137.0f, 130.0f);
  Curve c_reverb_color_curve_2(29.0f, 29.0f, 85.0f);

  inline float interpolRT(float fract, float sample_tm1, float sample_t0, float sample_tp1, float sample_tp2)
  {
    float fract_square = fract * fract;
    float fract_cube = fract_square * fract;

    float a = 0.5f * (sample_tp1 - sample_tm1);
    float b = 0.5f * (sample_tp2 - sample_t0);
    float c = sample_t0 - sample_tp1;

    return sample_t0 + fract * a + fract_cube * (a + b + 2.f * c) - fract_square * (2.f * a + b + 3.f * c);
  }

  Reverb::Reverb()
  {
    m_out_L = 0.0f;
    m_out_R = 0.0f;
    m_out_dry = 0.0f;
    m_out_wet = 0.0f;

    m_slow_tick = 0;
    m_slow_thrsh = 2 - 1;

    //**************************** Delay Offsets *****************************//
    // taking sample rate into account now
    // equal to reaktor implementation, the primes are reduced by one (reason unknown)
    unsigned index = 0;
    for(unsigned offset : s_offset_left)
    {
      m_offset_left[index++] = (offset) -1;
    }
    index = 0;
    for(unsigned offset : s_offset_right)
    {
      m_offset_right[index++] = (offset) -1;
    }

    //************************** Reverb Modulation ***************************//
    m_mod_1a = 0.0f;
    m_mod_2a = 0.0f;
    m_mod_1b = 0.0f;
    m_mod_2b = 0.0f;
    m_lfo_omega_1 = 0.86306f * 2.0f / SAMPLERATE;
    m_lfo_omega_2 = 0.6666f * 2.0f / SAMPLERATE;

    //****************************** Loop Filter *****************************//
    m_warpConst_PI = M_PI / SAMPLERATE;
    m_omegaClip_max = SAMPLERATE / 2.0f;

    m_lp_stateVar_L = 0.0f;
    m_lp_stateVar_R = 0.0f;
    m_hp_stateVar_L = 0.0f;
    m_hp_stateVar_R = 0.0f;

    //***************************** Delay Buffer *****************************//
    m_buffer_indx = 0;

    const unsigned buffer_size = REVERB_BUFFER_SIZE;

    m_buffer_sz_m1 = buffer_size - 1;
    m_buffer_sz_m2 = buffer_size - 2;

    m_buffer_L.resize(buffer_size);
    m_buffer_L1.resize(buffer_size);
    m_buffer_L2.resize(buffer_size);
    m_buffer_L3.resize(buffer_size);
    m_buffer_L4.resize(buffer_size);
    m_buffer_L5.resize(buffer_size);
    m_buffer_L6.resize(buffer_size);
    m_buffer_L7.resize(buffer_size);
    m_buffer_L8.resize(buffer_size);
    m_buffer_L9.resize(buffer_size);

    m_buffer_R.resize(buffer_size);
    m_buffer_R1.resize(buffer_size);
    m_buffer_R2.resize(buffer_size);
    m_buffer_R3.resize(buffer_size);
    m_buffer_R4.resize(buffer_size);
    m_buffer_R5.resize(buffer_size);
    m_buffer_R6.resize(buffer_size);
    m_buffer_R7.resize(buffer_size);
    m_buffer_R8.resize(buffer_size);
    m_buffer_R9.resize(buffer_size);

    std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.0f);
    std::fill(m_buffer_L1.begin(), m_buffer_L1.end(), 0.0f);
    std::fill(m_buffer_L2.begin(), m_buffer_L2.end(), 0.0f);
    std::fill(m_buffer_L3.begin(), m_buffer_L3.end(), 0.0f);
    std::fill(m_buffer_L4.begin(), m_buffer_L4.end(), 0.0f);
    std::fill(m_buffer_L5.begin(), m_buffer_L5.end(), 0.0f);
    std::fill(m_buffer_L6.begin(), m_buffer_L6.end(), 0.0f);
    std::fill(m_buffer_L7.begin(), m_buffer_L7.end(), 0.0f);
    std::fill(m_buffer_L8.begin(), m_buffer_L8.end(), 0.0f);
    std::fill(m_buffer_L9.begin(), m_buffer_L9.end(), 0.0f);

    std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.0f);
    std::fill(m_buffer_R1.begin(), m_buffer_R1.end(), 0.0f);
    std::fill(m_buffer_R2.begin(), m_buffer_R2.end(), 0.0f);
    std::fill(m_buffer_R3.begin(), m_buffer_R3.end(), 0.0f);
    std::fill(m_buffer_R4.begin(), m_buffer_R4.end(), 0.0f);
    std::fill(m_buffer_R5.begin(), m_buffer_R5.end(), 0.0f);
    std::fill(m_buffer_R6.begin(), m_buffer_R6.end(), 0.0f);
    std::fill(m_buffer_R7.begin(), m_buffer_R7.end(), 0.0f);
    std::fill(m_buffer_R8.begin(), m_buffer_R8.end(), 0.0f);
    std::fill(m_buffer_R9.begin(), m_buffer_R9.end(), 0.0f);

    m_stateVar_L1 = 0.0f;
    m_stateVar_L2 = 0.0f;
    m_stateVar_L3 = 0.0f;
    m_stateVar_L4 = 0.0f;
    m_stateVar_L5 = 0.0f;
    m_stateVar_L6 = 0.0f;
    m_stateVar_L7 = 0.0f;
    m_stateVar_L8 = 0.0f;
    m_stateVar_L9 = 0.0f;

    m_stateVar_R1 = 0.0f;
    m_stateVar_R2 = 0.0f;
    m_stateVar_R3 = 0.0f;
    m_stateVar_R4 = 0.0f;
    m_stateVar_R5 = 0.0f;
    m_stateVar_R6 = 0.0f;
    m_stateVar_R7 = 0.0f;
    m_stateVar_R8 = 0.0f;
    m_stateVar_R9 = 0.0f;

    //******************************* Smoothing ******************************//
    m_smooth_inc = 1.0f / std::max(50.0f * (0.001f * SAMPLERATE / 2.0f), 1.e-12f);  /// 50ms als DEfine Bitte!
    // todo: pack settings like this into c15_config.h (for example)
  }

  static float pitchToFrequency(float pitch)
  {
    return 440.f * powf(2, (pitch - 69.f) / 12.f);
  }

  void Reverb::set(float size, float chorus, float bal, float pre, float color)
  {

    auto lpf = std::clamp(pitchToFrequency(c_reverb_color_curve_1.applyCurve(color)), 0.f, SAMPLERATE / 2.f);
    auto hpf = std::clamp(pitchToFrequency(c_reverb_color_curve_2.applyCurve(color)), 0.f, SAMPLERATE / 2.f);

    float tmpVar;
    float tmp_target;
    tmpVar = size;
    tmp_target = chorus * (tmpVar * -200.0f + 311.0f);
    if(m_depth_target - tmp_target != 0.0f)
    {
      m_depth_target = tmp_target;
      m_depth_base = m_depth;
      m_depth_diff = m_depth_target - m_depth_base;
      m_depth_ramp = 0.0f;
    }

    tmp_target = tmpVar * (0.5f - std::abs(tmpVar) * -0.5f);
    if(m_size_target - tmp_target != 0.0f)
    {
      m_size_target = tmp_target;
      m_size_base = m_size;
      m_size_diff = m_size_target - m_size_base;
      m_size_ramp = 0.0f;
    }

    tmp_target = bal;
    if(m_bal_target - tmp_target != 0.0f)
    {
      m_bal_target = tmp_target;
      m_bal_base = m_bal;
      m_bal_diff = m_bal_target - m_bal_base;
      m_bal_ramp = 0.0f;
    }

    tmpVar = pre;
    tmp_target = std::round(tmpVar);
    if(m_preDel_L_target - tmp_target != 0.0f)
    {
      m_preDel_L_target = tmp_target;
      m_preDel_L_base = m_preDel_L;
      m_preDel_L_diff = m_preDel_L_target - m_preDel_L_base;
      m_preDel_L_ramp = 0.0f;
    }

    tmp_target = std::round(tmpVar * 1.18933f);
    if(m_preDel_R_target - tmp_target != 0.0f)
    {
      m_preDel_R_target = tmp_target;
      m_preDel_R_base = m_preDel_R;
      m_preDel_R_diff = m_preDel_R_target - m_preDel_R_base;
      m_preDel_R_ramp = 0.0f;
    }

    tmp_target = std::clamp(lpf, 0.1f, m_omegaClip_max);
    tmp_target = std::tan(tmp_target * m_warpConst_PI);
    if(m_lp_omega_target - tmp_target != 0.0f)
    {
      m_lp_omega_target = tmp_target;
      m_lp_omega_base = m_lp_omega;
      m_lp_omega_diff = m_lp_omega_target - m_lp_omega_base;
      m_lp_omega_ramp = 0.0f;
    }

    tmp_target = std::clamp(hpf, 0.1f, m_omegaClip_max);
    tmp_target = std::tan(tmp_target * m_warpConst_PI);
    if(m_hp_omega_target - tmp_target != 0.0f)
    {
      m_hp_omega_target = tmp_target;
      m_hp_omega_base = m_hp_omega;
      m_hp_omega_diff = m_hp_omega_target - m_hp_omega_base;
      m_hp_omega_ramp = 0.0f;
    }
  }

  void Reverb::apply(float _rawSample_L, float _rawSample_R, float send, float feedback, float dry, float wet)
  {
    float tmpVar;
    int32_t ind_t0, ind_tm1, ind_tp1, ind_tp2;
    float wetSample_L, wetSample_R;

    //************************** Reverb Modulation ***************************//
    if(!m_slow_tick)
    {
      if(m_depth_ramp > 1.0f)  // Depth Smth.
      {
        m_depth = m_depth_target;
      }
      else
      {
        m_depth = m_depth_base + (m_depth_diff * m_depth_ramp);
        m_depth_ramp += m_smooth_inc;
      }

      if(m_size_ramp > 1.0f)  // Size Smth.
      {
        m_size = m_size_target;
      }
      else
      {
        m_size = m_size_base + (m_size_diff * m_size_ramp);
        m_size_ramp += m_smooth_inc;
      }
      m_absorb = m_size * 0.334f + 0.666f;
      m_fb_amnt = m_size * 0.667f + 0.333f;

      if(m_bal_ramp > 1.0f)  // Balance Smth.
      {
        m_bal = m_bal_target;
      }
      else
      {
        m_bal = m_bal_base + (m_bal_diff * m_bal_ramp);
        m_bal_ramp += m_smooth_inc;
      }
      tmpVar = m_bal;
      m_bal_full = tmpVar * (2.0f - tmpVar);
      tmpVar = 1.0f - tmpVar;
      m_bal_half = tmpVar * (2.0f - tmpVar);

      if(m_preDel_L_ramp > 1.0f)  // PreDelay L Smth.
      {
        m_preDel_L = m_preDel_L_target;
      }
      else
      {
        m_preDel_L = m_preDel_L_base + (m_preDel_L_diff * m_preDel_L_ramp);
        m_preDel_L_ramp += m_smooth_inc;
      }

      if(m_preDel_R_ramp > 1.0f)  // PreDelay R Smth.
      {
        m_preDel_R = m_preDel_R_target;
      }
      else
      {
        m_preDel_R = m_preDel_R_base + (m_preDel_R_diff * m_preDel_R_ramp);
        m_preDel_R_ramp += m_smooth_inc;
      }

      if(m_lp_omega_ramp > 1.0f)  // LP Omega Smth.
      {
        m_lp_omega = m_lp_omega_target;
      }
      else
      {
        m_lp_omega = m_lp_omega_base + (m_lp_omega_diff * m_lp_omega_ramp);
        m_lp_omega_ramp += m_smooth_inc;
      }
      m_lp_a0 = 1.0f / (m_lp_omega + 1.0f);
      m_lp_a1 = m_lp_omega - 1.0f;

      if(m_hp_omega_ramp > 1.0f)  // HP Omega Smth.
      {
        m_hp_omega = m_hp_omega_target;
      }
      else
      {
        m_hp_omega = m_hp_omega_base + (m_hp_omega_diff * m_hp_omega_ramp);
        m_hp_omega_ramp += m_smooth_inc;
      }
      m_hp_a0 = 1.0f / (m_hp_omega + 1.0f);
      m_hp_a1 = m_hp_omega - 1.0f;
      tmpVar = m_lfo_stateVar_1 + m_lfo_omega_1;
      tmpVar = tmpVar - std::round(tmpVar);
      m_lfo_stateVar_1 = tmpVar;

      tmpVar = (8.0f - std::abs(tmpVar) * 16.0f) * tmpVar;
      tmpVar += 1.0f;
      m_mod_1a = tmpVar * m_depth;
      m_mod_2a = (1.0f - tmpVar) * m_depth;

      tmpVar = m_lfo_stateVar_2 + m_lfo_omega_2;
      tmpVar = tmpVar - std::round(tmpVar);
      m_lfo_stateVar_2 = tmpVar;

      tmpVar = (8.0f - std::abs(tmpVar) * 16.0f) * tmpVar;
      tmpVar += 1.0f;
      m_mod_1b = tmpVar * m_depth;
      m_mod_2b = (1.0f - tmpVar) * m_depth;
    }

    m_slow_tick = (m_slow_tick + 1) & m_slow_thrsh;
    //************************************************************************//
    //**************************** Left Channel ******************************//
    wetSample_L = _rawSample_L * send * feedback;

    //****************************** Asym 2 L ********************************//
    m_buffer_L[m_buffer_indx] = wetSample_L;

    tmpVar = std::clamp(m_preDel_L, 0.0f, static_cast<float>(m_buffer_sz_m1));  /// this can be in a setter!

    ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
    tmpVar = tmpVar - static_cast<float>(ind_t0);

    ind_tm1 = ind_t0 + 1;

    ind_t0 = m_buffer_indx - ind_t0;
    ind_tm1 = m_buffer_indx - ind_tm1;

    ind_t0 &= m_buffer_sz_m1;
    ind_tm1 &= m_buffer_sz_m1;

    wetSample_L = m_buffer_L[ind_t0] + tmpVar * (m_buffer_L[ind_tm1] - m_buffer_L[ind_t0]);

    wetSample_L += (m_stateVar_R9 * m_fb_amnt);

    //**************************** Loop Filter L *****************************//
    wetSample_L = (wetSample_L - m_lp_stateVar_L * m_lp_a1) * m_lp_a0;
    tmpVar = m_lp_stateVar_L;
    m_lp_stateVar_L = wetSample_L;

    wetSample_L = (wetSample_L + tmpVar) * m_lp_omega;

    wetSample_L = (wetSample_L - m_hp_stateVar_L * m_hp_a1) * m_hp_a0;
    tmpVar = m_hp_stateVar_L;
    m_hp_stateVar_L = wetSample_L;

    wetSample_L = wetSample_L - tmpVar;

    //****************************** Del 4p L1 *******************************//
    tmpVar = m_stateVar_L1 * m_absorb;
    wetSample_L += (tmpVar * REV_G_1);

    m_buffer_L1[m_buffer_indx] = wetSample_L;

    wetSample_L = wetSample_L * -REV_G_1 + tmpVar;

    tmpVar = m_offset_left[Offset_1] + m_mod_2a;
    tmpVar = std::clamp(tmpVar, 1.0f, static_cast<float>(m_buffer_sz_m2));

    ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
    tmpVar = tmpVar - static_cast<float>(ind_t0);

    ind_tm1 = std::max(ind_t0, 1) - 1;
    ind_tp1 = ind_t0 + 1;
    ind_tp2 = ind_t0 + 2;

    ind_tm1 = m_buffer_indx - ind_tm1;
    ind_t0 = m_buffer_indx - ind_t0;
    ind_tp1 = m_buffer_indx - ind_tp1;
    ind_tp2 = m_buffer_indx - ind_tp2;

    ind_tm1 &= m_buffer_sz_m1;
    ind_t0 &= m_buffer_sz_m1;
    ind_tp1 &= m_buffer_sz_m1;
    ind_tp2 &= m_buffer_sz_m1;

    m_stateVar_L1
        = interpolRT(tmpVar, m_buffer_L1[ind_tm1], m_buffer_L1[ind_t0], m_buffer_L1[ind_tp1], m_buffer_L1[ind_tp2]);

    //***************************** Del 1p L2 ******************************//
    tmpVar = m_stateVar_L2 * m_absorb;
    wetSample_L = wetSample_L + (tmpVar * REV_G_2);

    m_buffer_L2[m_buffer_indx] = wetSample_L;

    wetSample_L = wetSample_L * -REV_G_2 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_left[Offset_2];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_L2 = m_buffer_L2[ind_t0];

    //***************************** Del 1p L3 ******************************//
    tmpVar = m_stateVar_L3 * m_absorb;
    wetSample_L = wetSample_L + (tmpVar * REV_G_3);

    m_buffer_L3[m_buffer_indx] = wetSample_L;

    wetSample_L = wetSample_L * -REV_G_3 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_left[Offset_3];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_L3 = m_buffer_L3[ind_t0];

    //***************************** Del 1p L4 ******************************//
    tmpVar = m_stateVar_L4 * m_absorb;
    wetSample_L = wetSample_L + (tmpVar * REV_G_4);

    m_buffer_L4[m_buffer_indx] = wetSample_L;

    float wetSample_L2 = wetSample_L * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_left[Offset_4];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_L4 = m_buffer_L4[ind_t0];

    //***************************** Del 1p L5 ******************************//
    tmpVar = m_stateVar_L5 * m_absorb;
    wetSample_L = wetSample_L2 + (tmpVar * REV_G_4);

    m_buffer_L5[m_buffer_indx] = wetSample_L;

    wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_left[Offset_5];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_L5 = m_buffer_L5[ind_t0];

    //***************************** Del 1p L6 ******************************//
    tmpVar = m_stateVar_L6 * m_absorb;
    wetSample_L = wetSample_L + (tmpVar * REV_G_4);

    m_buffer_L6[m_buffer_indx] = wetSample_L;

    wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_left[Offset_6];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_L6 = m_buffer_L6[ind_t0];

    //***************************** Del 1p L7 ******************************//
    tmpVar = m_stateVar_L7 * m_absorb;
    wetSample_L = wetSample_L + (tmpVar * REV_G_4);

    m_buffer_L7[m_buffer_indx] = wetSample_L;

    wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_left[Offset_7];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_L7 = m_buffer_L7[ind_t0];

    //***************************** Del 1p L8 ******************************//
    tmpVar = m_stateVar_L8 * m_absorb;
    wetSample_L = wetSample_L + (tmpVar * REV_G_4);

    m_buffer_L8[m_buffer_indx] = wetSample_L;

    wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_left[Offset_8];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_L8 = m_buffer_L8[ind_t0];

    //************************************************************************//
    //*************************** Right Channel ******************************//
    wetSample_R = _rawSample_R * send * feedback;

    //****************************** Asym 2 R ********************************//
    m_buffer_R[m_buffer_indx] = wetSample_R;

    tmpVar = std::clamp(m_preDel_R, 0.0f, static_cast<float>(m_buffer_sz_m1));

    ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
    tmpVar = tmpVar - static_cast<float>(ind_t0);

    ind_tm1 = ind_t0 + 1;

    ind_t0 = m_buffer_indx - ind_t0;
    ind_tm1 = m_buffer_indx - ind_tm1;

    ind_t0 &= m_buffer_sz_m1;
    ind_tm1 &= m_buffer_sz_m1;

    wetSample_R = m_buffer_R[ind_t0] + tmpVar * (m_buffer_R[ind_tm1] - m_buffer_R[ind_t0]);

    wetSample_R += (m_stateVar_L9 * m_fb_amnt);

    //**************************** Loop Filter R *****************************//
    wetSample_R = (wetSample_R - m_lp_stateVar_R * m_lp_a1) * m_lp_a0;
    tmpVar = m_lp_stateVar_R;
    m_lp_stateVar_R = wetSample_R;

    wetSample_R = (wetSample_R + tmpVar) * m_lp_omega;

    wetSample_R = (wetSample_R - m_hp_stateVar_R * m_hp_a1) * m_hp_a0;
    tmpVar = m_hp_stateVar_R;
    m_hp_stateVar_R = wetSample_R;

    wetSample_R = wetSample_R - tmpVar;

    //****************************** Del 4p R1 *******************************//
    tmpVar = m_stateVar_R1 * m_absorb;
    wetSample_R += (tmpVar * REV_G_1);

    m_buffer_R1[m_buffer_indx] = wetSample_R;

    wetSample_R = wetSample_R * -REV_G_1 + tmpVar;

    tmpVar = m_offset_right[Offset_1] + m_mod_2b;
    tmpVar = std::clamp(tmpVar, 1.0f, static_cast<float>(m_buffer_sz_m2));

    ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
    tmpVar = tmpVar - static_cast<float>(ind_t0);

    ind_tm1 = std::max(ind_t0, 1) - 1;
    ind_tp1 = ind_t0 + 1;
    ind_tp2 = ind_t0 + 2;

    ind_tm1 = m_buffer_indx - ind_tm1;
    ind_t0 = m_buffer_indx - ind_t0;
    ind_tp1 = m_buffer_indx - ind_tp1;
    ind_tp2 = m_buffer_indx - ind_tp2;

    ind_tm1 &= m_buffer_sz_m1;
    ind_t0 &= m_buffer_sz_m1;
    ind_tp1 &= m_buffer_sz_m1;
    ind_tp2 &= m_buffer_sz_m1;

    m_stateVar_R1
        = interpolRT(tmpVar, m_buffer_R1[ind_tm1], m_buffer_R1[ind_t0], m_buffer_R1[ind_tp1], m_buffer_R1[ind_tp2]);

    //***************************** Del 1p R2 ******************************//
    tmpVar = m_stateVar_R2 * m_absorb;
    wetSample_R = wetSample_R + (tmpVar * REV_G_2);

    m_buffer_R2[m_buffer_indx] = wetSample_R;

    wetSample_R = wetSample_R * -REV_G_2 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_right[Offset_2];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_R2 = m_buffer_R2[ind_t0];

    //***************************** Del 1p R3 ******************************//
    tmpVar = m_stateVar_R3 * m_absorb;
    wetSample_R = wetSample_R + (tmpVar * REV_G_3);

    m_buffer_R3[m_buffer_indx] = wetSample_R;

    wetSample_R = wetSample_R * -REV_G_3 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_right[Offset_3];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_R3 = m_buffer_R3[ind_t0];

    //***************************** Del 1p R4 ******************************//
    tmpVar = m_stateVar_R4 * m_absorb;
    wetSample_R = wetSample_R + (tmpVar * REV_G_4);

    m_buffer_R4[m_buffer_indx] = wetSample_R;

    float wetSample_R2 = wetSample_R * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_right[Offset_4];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_R4 = m_buffer_R4[ind_t0];

    //***************************** Del 1p R5 ******************************//
    tmpVar = m_stateVar_R5 * m_absorb;
    wetSample_R = wetSample_R2 + (tmpVar * REV_G_4);

    m_buffer_R5[m_buffer_indx] = wetSample_R;

    wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_right[Offset_5];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_R5 = m_buffer_R5[ind_t0];

    //***************************** Del 1p R6 ******************************//
    tmpVar = m_stateVar_R6 * m_absorb;
    wetSample_R = wetSample_R + (tmpVar * REV_G_4);

    m_buffer_R6[m_buffer_indx] = wetSample_R;

    wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_right[Offset_6];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_R6 = m_buffer_R6[ind_t0];

    //***************************** Del 1p R7 ******************************//
    tmpVar = m_stateVar_R7 * m_absorb;
    wetSample_R = wetSample_R + (tmpVar * REV_G_4);

    m_buffer_R7[m_buffer_indx] = wetSample_R;

    wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_right[Offset_7];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_R7 = m_buffer_R7[ind_t0];

    //***************************** Del 1p R8 ******************************//
    tmpVar = m_stateVar_R8 * m_absorb;
    wetSample_R = wetSample_R + (tmpVar * REV_G_4);

    m_buffer_R8[m_buffer_indx] = wetSample_R;

    wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

    ind_t0 = m_buffer_indx - m_offset_right[Offset_8];
    ind_t0 &= m_buffer_sz_m1;

    m_stateVar_R8 = m_buffer_R8[ind_t0];

    //************************************************************************//
    //*************************** Feedback Delay *****************************//
    //****************************** Del 4p L9 *******************************//
    m_buffer_L9[m_buffer_indx] = wetSample_L;

    tmpVar = m_offset_left[Offset_9] + m_mod_1a;
    tmpVar = std::clamp(tmpVar, 0.0f, static_cast<float>(m_buffer_sz_m2));

    ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
    tmpVar = tmpVar - static_cast<float>(ind_t0);

    ind_tm1 = std::max(ind_t0, 1) - 1;
    ind_tp1 = ind_t0 + 1;
    ind_tp2 = ind_t0 + 2;

    ind_tm1 = m_buffer_indx - ind_tm1;
    ind_t0 = m_buffer_indx - ind_t0;
    ind_tp1 = m_buffer_indx - ind_tp1;
    ind_tp2 = m_buffer_indx - ind_tp2;

    ind_tm1 &= m_buffer_sz_m1;
    ind_t0 &= m_buffer_sz_m1;
    ind_tp1 &= m_buffer_sz_m1;
    ind_tp2 &= m_buffer_sz_m1;

    m_stateVar_L9
        = interpolRT(tmpVar, m_buffer_L9[ind_tm1], m_buffer_L9[ind_t0], m_buffer_L9[ind_tp1], m_buffer_L9[ind_tp2]);

    //***************************** Del 4p R9 ******************************//
    m_buffer_R9[m_buffer_indx] = wetSample_R;

    tmpVar = m_offset_right[Offset_9] + m_mod_1b;
    tmpVar = std::clamp(tmpVar, 0.0f, static_cast<float>(m_buffer_sz_m2));

    ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
    tmpVar = tmpVar - static_cast<float>(ind_t0);

    ind_tm1 = std::max(ind_t0, 1) - 1;
    ind_tp1 = ind_t0 + 1;
    ind_tp2 = ind_t0 + 2;

    ind_tm1 = m_buffer_indx - ind_tm1;
    ind_t0 = m_buffer_indx - ind_t0;
    ind_tp1 = m_buffer_indx - ind_tp1;
    ind_tp2 = m_buffer_indx - ind_tp2;

    ind_tm1 &= m_buffer_sz_m1;
    ind_t0 &= m_buffer_sz_m1;
    ind_tp1 &= m_buffer_sz_m1;
    ind_tp2 &= m_buffer_sz_m1;

    m_stateVar_R9
        = interpolRT(tmpVar, m_buffer_R9[ind_tm1], m_buffer_R9[ind_t0], m_buffer_R9[ind_tp1], m_buffer_R9[ind_tp2]);

    m_buffer_indx = (m_buffer_indx + 1) & m_buffer_sz_m1;

    //************************************************************************//
    //**************************** Output Mixer ******************************//
    wetSample_L = wetSample_L * m_bal_full + wetSample_L2 * m_bal_half;
    wetSample_R = wetSample_R * m_bal_full + wetSample_R2 * m_bal_half;

    m_out_L = _rawSample_L * dry + wetSample_L * wet;
    m_out_R = _rawSample_R * dry + wetSample_R * wet;

    m_out_dry = _rawSample_L + _rawSample_R;
    m_out_wet = wetSample_L + wetSample_R;
  }

  void Reverb::resetDSP()
  {
    m_out_L = 0.0f;
    m_out_R = 0.0f;
    m_out_dry = 0.0f;
    m_out_wet = 0.0f;

    //****************************** Loop Filter *****************************//

    m_lp_stateVar_L = 0.0f;
    m_lp_stateVar_R = 0.0f;
    m_hp_stateVar_L = 0.0f;
    m_hp_stateVar_R = 0.0f;

    //***************************** Delay Buffer *****************************//

    std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.0f);
    std::fill(m_buffer_L1.begin(), m_buffer_L1.end(), 0.0f);
    std::fill(m_buffer_L2.begin(), m_buffer_L2.end(), 0.0f);
    std::fill(m_buffer_L3.begin(), m_buffer_L3.end(), 0.0f);
    std::fill(m_buffer_L4.begin(), m_buffer_L4.end(), 0.0f);
    std::fill(m_buffer_L5.begin(), m_buffer_L5.end(), 0.0f);
    std::fill(m_buffer_L6.begin(), m_buffer_L6.end(), 0.0f);
    std::fill(m_buffer_L7.begin(), m_buffer_L7.end(), 0.0f);
    std::fill(m_buffer_L8.begin(), m_buffer_L8.end(), 0.0f);
    std::fill(m_buffer_L9.begin(), m_buffer_L9.end(), 0.0f);

    std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.0f);
    std::fill(m_buffer_R1.begin(), m_buffer_R1.end(), 0.0f);
    std::fill(m_buffer_R2.begin(), m_buffer_R2.end(), 0.0f);
    std::fill(m_buffer_R3.begin(), m_buffer_R3.end(), 0.0f);
    std::fill(m_buffer_R4.begin(), m_buffer_R4.end(), 0.0f);
    std::fill(m_buffer_R5.begin(), m_buffer_R5.end(), 0.0f);
    std::fill(m_buffer_R6.begin(), m_buffer_R6.end(), 0.0f);
    std::fill(m_buffer_R7.begin(), m_buffer_R7.end(), 0.0f);
    std::fill(m_buffer_R8.begin(), m_buffer_R8.end(), 0.0f);
    std::fill(m_buffer_R9.begin(), m_buffer_R9.end(), 0.0f);

    m_stateVar_L1 = 0.0f;
    m_stateVar_L2 = 0.0f;
    m_stateVar_L3 = 0.0f;
    m_stateVar_L4 = 0.0f;
    m_stateVar_L5 = 0.0f;
    m_stateVar_L6 = 0.0f;
    m_stateVar_L7 = 0.0f;
    m_stateVar_L8 = 0.0f;
    m_stateVar_L9 = 0.0f;

    m_stateVar_R1 = 0.0f;
    m_stateVar_R2 = 0.0f;
    m_stateVar_R3 = 0.0f;
    m_stateVar_R4 = 0.0f;
    m_stateVar_R5 = 0.0f;
    m_stateVar_R6 = 0.0f;
    m_stateVar_R7 = 0.0f;
    m_stateVar_R8 = 0.0f;
    m_stateVar_R9 = 0.0f;
  }
}