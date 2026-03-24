#pragma once
#include "Synth.h"
#include "DaisyDuino.h"

#define MAX_SYNTHS 3

struct SynthCfg {
  Synth::Config config;
  float volume = 1.0f;
};

class Pad {
  public:
    struct Config {
      SynthCfg layers[MAX_SYNTHS];
      int active_layers_count;
    };

    void Init(float sr) {
      active_count = 0;
      for (int i = 0; i < MAX_SYNTHS; i++) {
        synths[i].Init(sr);
      }
    }

    void ApplyConfig(const Config& config, int midi_root) {
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
        for (int i = 0; i < active_count; i++) {
            synths[i].Process(l, r);
        }
    }

    bool isRunning() {
      for (int i = 0; i < active_count; i++) {
        if (synths[i].isRunning()) return true;
      }
      return false;
    }

  private:
    Synth synths[MAX_SYNTHS];
    int active_count;
};