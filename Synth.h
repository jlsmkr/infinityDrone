#pragma once
#include "DaisyDuino.h"
#include "Filter.h"
#include "array"
#include "vector"

struct EnvelopeCfg {
  float attack;
  float decay;
  float sustain;
  float release;
};

struct SvfCfg {
  float freq;
  float res;
};

struct KeyCfg {
  int midi;
  std::array<float, 2> detune;
  uint8_t waveform;
  float amp;
};

struct EqCfg {
  float freq;
  float Q;
  float gain_db;
};

class Synth
{
  public:
    struct Config {
      std::vector<KeyCfg> keys;
      EnvelopeCfg env_main;
      SvfCfg filter;
      SvfCfg eq_lc;
      EqCfg eq_bell;
    };

    static const int MAX_OSC = 10;

    Synth(float samplerate, const Config& config) 
      : settings(config),
        samplerate(samplerate)
    {
      initOsc();
      initAdsr(env_main, settings.env_main);
      initSvf(filter, settings.filter);
      initSvf(eq_lc, settings.eq_lc);
      eq_bell.Init(samplerate);
      eq_bell.SetPeak(settings.eq_bell.freq, settings.eq_bell.Q, settings.eq_bell.gain_db);
    }

    std::array<float, 2> Process() {
      float signal = 0;

      if (!isRunning() && !gate_state) return {0.0f, 0.0f};

      for (int i = 0; i < num_osc; i++) {
        signal += osc[i].Process();
      }

      float env_val_main = env_main.Process(gate_state);

      signal = (signal * 0.5f) * env_val_main;

      filter.Process(signal);
      signal = filter.Low();
      signal = eq_bell.Process(signal);
      eq_lc.Process(signal);
      signal = eq_lc.High();

      float revL, revR;
      revL = 0;
      revR = 0;
      return {
        (signal * 0.35) + (revL * 0.65),
        (signal * 0.35) + (revR * 0.65)
      };
    }

    void SetGate(bool gate) {
      gate_state = gate;
    }

    bool isRunning() {
      return env_main.IsRunning();
    }

  private:
    Config settings;
    float samplerate;

    Oscillator osc[MAX_OSC];
    Adsr env_main;
    Svf filter;
    Svf eq_lc;
    Filter eq_bell;

    bool gate_state = false;

    int num_osc;

    void initOsc() {
      int index = 0;
      for (const auto& key : settings.keys) {
        for (int d = 0; d < 2; d++) {
          if (index >= MAX_OSC) break;

          osc[index].Init(samplerate);
          osc[index].SetWaveform(key.waveform);
          osc[index].SetAmp(key.amp);
          osc[index].SetFreq(daisysp::mtof((float)key.midi) * key.detune[d]);

          index++;
        }
      }
      num_osc = index;
    }

    void initAdsr(Adsr &env, const EnvelopeCfg& envCfg) {
      env.Init(samplerate);
      env.SetTime(ADSR_SEG_ATTACK, envCfg.attack);
      env.SetTime(ADSR_SEG_DECAY, envCfg.decay);
      env.SetSustainLevel(envCfg.sustain);
      env.SetTime(ADSR_SEG_RELEASE, envCfg.release);
      
      env.Process(true);

      env.Retrigger(false);
    }

    void initSvf(Svf &svf, const SvfCfg& filterCfg) {
      svf.Init(samplerate);
      svf.SetFreq(filterCfg.freq);
      svf.SetRes(filterCfg.res);
    }
};