#pragma once
#include "DaisyDuino.h"
#include "Filter.h"

#define MAX_KEYS 4
#define MAX_OSC 8

struct Note {
  int interval;
  float detune;
  uint8_t waveform;
  float amp;
};

struct EnvelopeCfg { float attack, decay, sustain, release; };
struct SvfCfg { float freq, res; };
struct EqCfg { float freq, Q, gain_db; };
struct LFOCfg { float freq, amp; bool active; };

class Synth
{
  public:
    struct Config {
      Note notes[MAX_KEYS];
      int active_keys_count;
      EnvelopeCfg env_main;
      SvfCfg eq_lpf;
      SvfCfg eq_hpf;
      EqCfg eq_bell1;
      EqCfg eq_bell2;
      LFOCfg lfo_amp;
      LFOCfg lfo_lpf;
    };
    bool gate_state = false;
    void Init(float sr) {
      env_main.Init(sr);
      eq_lpf.Init(sr);
      eq_hpf.Init(sr);
      eq_bell1.Init(sr);
      eq_bell2.Init(sr);
      lfo_amp.Init(sr);
      lfo_amp.SetWaveform(Oscillator::WAVE_SIN);
      lfo_lpf.Init(sr);
      lfo_lpf.SetWaveform(Oscillator::WAVE_SIN);
      lfo_lpf.SetAmp(1.0f);

      for (int i = 0; i < MAX_OSC; i++) {
        osc[i].Init(sr);
      }
    }

    void ApplyConfig(const Config& config, int& midi_root) {
      settings = config;
      env_main.SetTime(ADSR_SEG_ATTACK, config.env_main.attack);
      env_main.SetTime(ADSR_SEG_DECAY, config.env_main.decay);
      env_main.SetSustainLevel(config.env_main.sustain);
      env_main.SetTime(ADSR_SEG_RELEASE, config.env_main.release);
      env_main.Process(true);
      env_main.Retrigger(false);

      eq_lpf.SetFreq(config.eq_lpf.freq);
      eq_lpf.SetRes(config.eq_lpf.res);

      eq_hpf.SetFreq(config.eq_hpf.freq);
      eq_hpf.SetRes(config.eq_hpf.res);

      eq_bell1.SetPeak(config.eq_bell1.freq, config.eq_bell1.Q, config.eq_bell1.gain_db);
      eq_bell2.SetPeak(config.eq_bell2.freq, config.eq_bell2.Q, config.eq_bell2.gain_db);

      if (config.lfo_amp.active) {
        lfo_amp.SetFreq(config.lfo_amp.freq);
        lfo_amp.SetAmp(config.lfo_amp.amp);
      }
      if (config.lfo_lpf.active) {
        lfo_lpf.SetFreq(config.lfo_lpf.freq);
      }
      

      active_osc_count = 0;
      for (int i = 0; i < config.active_keys_count; i++) {
        int midi = midi_root + config.notes[i].interval;

        osc[active_osc_count].SetWaveform(config.notes[i].waveform);
        osc[active_osc_count].SetAmp(config.notes[i].amp);
        osc[active_osc_count].SetFreq(daisysp::mtof(midi));

        active_osc_count++;

        osc[active_osc_count].SetWaveform(config.notes[i].waveform);
        osc[active_osc_count].SetAmp(config.notes[i].amp);
        osc[active_osc_count].SetFreq(daisysp::mtof(midi) * config.notes[i].detune);

        active_osc_count++;
      }
    }

    float Process() {
      if (!isRunning() && !gate_state) return 0.0f;

      float signal = 0.0f;
      for (int i = 0; i < active_osc_count; i++) {
        signal += osc[i].Process();
      }

      float env_main_val = env_main.Process(gate_state);
      signal = (signal / (float)active_osc_count) * env_main_val;

      if (settings.lfo_amp.active) {
        float lfo_amp_val = lfo_amp.Process();
        float trem = 1.0f + (lfo_amp_val * settings.lfo_amp.amp);
        signal *= trem;
      }

      if (settings.lfo_lpf.active) {
        float lfo_lpf_val = lfo_lpf.Process() * settings.lfo_lpf.amp;
        // the lfo amp setting represents the width the cutoff frequency is moving in each direction
        float cutoff = fclamp(settings.eq_lpf.freq + lfo_lpf_val, 20.0f, 18000.0f);
        eq_lpf.SetFreq(cutoff);
      }

      signal = signal * 0.5f;
      eq_lpf.Process(signal);
      signal = eq_lpf.Low();
      signal = eq_bell1.Process(signal);
      signal = eq_bell2.Process(signal);
      eq_hpf.Process(signal);
      signal = eq_hpf.High();

      return signal;
    }

    bool isRunning() {
      return env_main.IsRunning();
    }

  private:
    Oscillator osc[MAX_OSC];
    Oscillator lfo_amp, lfo_lpf;
    Adsr env_main;
    Svf eq_lpf;
    Svf eq_hpf;
    Filter eq_bell1, eq_bell2;
    int active_osc_count;
    Config settings;
};