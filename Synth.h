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

class Synth
{
  public:
    struct Config {
      Note notes[MAX_KEYS];
      int active_keys_count;
      EnvelopeCfg env_main;
      SvfCfg eq_hc;
      SvfCfg eq_lc;
      EqCfg eq_bell;
    };
    bool gate_state = false;
    void Init(float sr) {
      env_main.Init(sr);
      eq_hc.Init(sr);
      eq_lc.Init(sr);
      eq_bell.Init(sr);

      for (int i = 0; i < MAX_OSC; i++) {
        osc[i].Init(sr);
      }
    }

    void ApplyConfig(const Config& config, int& midi_root) {
      env_main.SetTime(ADSR_SEG_ATTACK, config.env_main.attack);
      env_main.SetTime(ADSR_SEG_DECAY, config.env_main.decay);
      env_main.SetSustainLevel(config.env_main.sustain);
      env_main.SetTime(ADSR_SEG_RELEASE, config.env_main.release);
      env_main.Process(true);
      env_main.Retrigger(false);

      eq_hc.SetFreq(config.eq_hc.freq);
      eq_hc.SetRes(config.eq_hc.res);

      eq_lc.SetFreq(config.eq_lc.freq);
      eq_lc.SetRes(config.eq_lc.res);

      eq_bell.SetPeak(config.eq_bell.freq, config.eq_bell.Q, config.eq_bell.gain_db);

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

    void Process(float &l, float &r) {
      if (!isRunning() && !gate_state) return;

      float signal = 0.0f;
      for (int i = 0; i < active_osc_count; i++) {
        signal += osc[i].Process();
      }

      float env_main_val = env_main.Process(gate_state);
      signal = (signal / (float)active_osc_count) * env_main_val;

      eq_hc.Process(signal);
      signal = eq_hc.Low();
      signal = eq_bell.Process(signal);
      eq_lc.Process(signal);
      signal = eq_lc.High();

      l += signal;
      r += signal;
    }

    bool isRunning() {
      return env_main.IsRunning();
    }

  private:
    Oscillator osc[MAX_OSC];
    Adsr env_main;
    Svf eq_hc;
    Svf eq_lc;
    Filter eq_bell;
    int active_osc_count;
};