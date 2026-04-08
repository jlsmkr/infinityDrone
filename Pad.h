#pragma once
#include "Synth.h"
#include "Filter.h"
#include "DaisyDuino.h"

#define MAX_SYNTHS 4

struct SynthCfg { Synth::Config config; float amp = 1.0f; };

class Pad {
  public:
    struct Config {
      SynthCfg layers[MAX_SYNTHS];
      int active_layers_count;
      EqCfg eq_bell1, eq_bell2;
      char name[5];
    };

    void Init(float sr) {
      eq_bell1.Init(sr);
      eq_bell2.Init(sr);
      active_count = 0;
      for (int i = 0; i < MAX_SYNTHS; i++) {
        synths[i].Init(sr);
      }
    }

    void ApplyConfig(const Config& config, int midi_root) {
      eq_bell1.SetPeak(config.eq_bell1.freq, config.eq_bell1.Q, config.eq_bell1.gain_db);
      eq_bell2.SetPeak(config.eq_bell2.freq, config.eq_bell2.Q, config.eq_bell2.gain_db);

      settings = config;
      active_count = config.active_layers_count;
      for (int i = 0; i < active_count; i++) {
        synths[i].ApplyConfig(config.layers[i].config, midi_root);
      }
    }

    void SetGate(bool gate_state) {
      for (int i = 0; i < active_count; i++) {
        synths[i].gate_state = gate_state;
      }
    }

    void Process(float &l, float &r) {
      float signal = 0.0f;
      for (int i = 0; i < active_count; i++) {
        signal += synths[i].Process() * settings.layers[i].amp;
      }

      signal = eq_bell1.Process(signal);
      signal = eq_bell2.Process(signal);

      l += signal;
      r += signal;
    }

    bool isRunning() {
      for (int i = 0; i < active_count; i++) {
        if (synths[i].isRunning()) return true;
      }
      return false;
    }

  private:
    Config settings;
    Synth synths[MAX_SYNTHS];
    int active_count;
    Filter eq_bell1, eq_bell2;
};