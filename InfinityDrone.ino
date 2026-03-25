#include "DaisyDuino.h"
#include "Synth.h"
#include "Pad.h"

#define PAD_COUNT 2

// PAD CFG

const Synth::Config deep_sub DSY_SDRAM_BSS = {
  .notes = {
    {-24, 1.001f, Oscillator::WAVE_SIN, 0.8f}, // A2
    {-17, 1.001f, Oscillator::WAVE_SIN, 1.0f}, // E3
  },
  .active_keys_count = 2,
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .eq_lpf    = {150.0f, 0.2f},
  .eq_hpf    = {50.0f, 0.2f},
  .eq_bell1   = {150.0f, 1.0f, -15.0f},
  .eq_bell2   = {500.0f, 1.5f, -5.0f},
  .lfo_amp  = {0.2f, 0.05f, true},
  .lfo_lpf  = {0.0f, 0.0f, false},
};

const Synth::Config deep_warm DSY_SDRAM_BSS {
  .notes = {
    {-15, 1.001f, Oscillator::WAVE_TRI, 0.2f}, // F#3
    {-12, 1.001f, Oscillator::WAVE_TRI, 1.0f}, // A3
    {-8, 1.001f, Oscillator::WAVE_TRI, 0.5f},  // C#4
    {-5, 1.001f, Oscillator::WAVE_TRI, 1.0f},  // E4
  },
  .active_keys_count = 4,
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .eq_lpf    = {350.0f, 0.2f},
  .eq_hpf    = {150.0f, 0.2f},
  .eq_bell1   = {250.0f, 0.8f, -20.0f},
  .eq_bell2   = {1000.0f, 1.5f, 12.0f},
  .lfo_amp  = {0.0f, 0.1f, false},
  .lfo_lpf  = {0.3f, 50.0f, false},
};

const Synth::Config deep_mid DSY_SDRAM_BSS {
  .notes = {
    {-10, 1.001f, Oscillator::WAVE_SAW, 0.8f}, // B3
    {-5, 1.001f, Oscillator::WAVE_SAW, 1.0f},  // E4
    //{-1, 1.001f, Oscillator::WAVE_SQUARE, 0.8f},  // G#4
    {2, 1.001f, Oscillator::WAVE_SAW, 0.5f},   // B4
  },
  .active_keys_count = 3,
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .eq_lpf    = {400.0f, 0.2f},
  .eq_hpf    = {400.0f, 0.2f},
  .eq_bell1   = {300.0f, 0.8f, -20.0f},
  .eq_bell2   = {2000.0f, 2.0f, 4.0f},
  .lfo_amp  = {0.0f, 0.0f, false},
  .lfo_lpf  = {0.3f, 50.0f, false},
};

const Synth::Config deep_shimmer DSY_SDRAM_BSS {
  .notes = {
    {14, 1.001f, Oscillator::WAVE_TRI, 0.4f}, // B5
    {18, 1.001f, Oscillator::WAVE_TRI, 0.3f}, // E6
    {26, 1.001f, Oscillator::WAVE_TRI, 0.2f}, // B6
  },
  .active_keys_count = 4,
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .eq_lpf    = {8000.0f, 0.2f},
  .eq_hpf    = {3000.0f, 0.2f},
  .eq_bell1   = {550.0f, 1.0f, -0.0f},
  .eq_bell2   = {550.0f, 1.0f, -0.0f},
  .lfo_amp  = {1.5f, 0.1f, true},
  .lfo_lpf  = {0.3f, 50.0f, false},
};

const Pad::Config pad1Cfg = {
  .layers = {
    {deep_sub, 0.4f},
    {deep_warm, 0.5f},
    {deep_mid, 0.1f},
    //{deep_shimmer, 0.2f},
  },
  .active_layers_count = 3,
  .eq_bell1   = {300.0f, 1.0f, -0.7f},
  .eq_bell2   = {800.0f, 1.5f, -4.0f},
};

const Pad::Config pad2Cfg = {
  .layers = {
    {deep_warm, 1.0f},
  },
  .active_layers_count = 1,
};

// MAIN

float sr;
int active_midi = 69; // 404 Hz
bool active_synth = false;
bool set_type = true; // true: midi; false: pad
int active_pad = 0;

Pad *pad1, *pad2;
static ReverbSc reverb;

Switch start_button, stop_button, set_button, prev_button, next_button;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float l = 0.0f;
    float r = 0.0f;

    pad1->Process(l, r);
    pad2->Process(l, r);

    float rev_l, rev_r;
    reverb.Process(l, r, &rev_l, &rev_r);

    out[0][i] = l * 0.25f + rev_l * 0.75f;
    out[1][i] = r * 0.25f + rev_r * 0.75f;
  }
}

void SwitchPad() {
  Pad::Config cfg;
  switch (active_pad) {
    case 0: cfg = pad1Cfg; break;
    default: cfg = pad2Cfg; break;
  }
  if (active_synth) {
    // no 1
    pad2->SetGate(false);
    pad1->ApplyConfig(cfg, active_midi);
    pad1->SetGate(true);
  } else {
    // no 2
    pad1->SetGate(false);
    pad2->ApplyConfig(cfg, active_midi);
    pad2->SetGate(true);
  }
}

void setup() {
  start_button.Init(1000, true, 15, INPUT_PULLUP);
  stop_button.Init(1000, true, 16, INPUT_PULLUP);
  set_button.Init(1000, true, 17, INPUT_PULLUP);
  prev_button.Init(1000, true, 18, INPUT_PULLUP);
  next_button.Init(1000, true, 19, INPUT_PULLUP);

  // init Daisy
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  sr = DAISY.get_samplerate();

  static Pad p1 DSY_SDRAM_BSS;
  static Pad p2 DSY_SDRAM_BSS;

  pad1 = &p1;
  pad2 = &p2;

  pad1->Init(sr);
  pad2->Init(sr);

  reverb.Init(sr);

  DAISY.begin(AudioCallback);
}

void loop() {
  start_button.Debounce();
  stop_button.Debounce();
  set_button.Debounce();
  prev_button.Debounce();
  next_button.Debounce();

  if (start_button.RisingEdge()) {
    active_synth = !active_synth; // just for testing purposes
    SwitchPad();
  }
  if (stop_button.RisingEdge()) {
    // stop everything
    pad1->SetGate(false);
    pad2->SetGate(false);
  }
  if (set_button.RisingEdge()) {
    set_type = !set_type;
  }
  if (prev_button.RisingEdge()) {
    if (set_type) {
      if (active_midi > 61) {
        active_midi--;
      } else {
        active_midi = 72;
      }
    } else {
      if (active_pad < (PAD_COUNT -1)) {
        active_pad++;
      } else {
        active_pad = 0;
      }
    }
  }
  if (next_button.RisingEdge()) {
    if (set_type) {
      if (active_midi < 72) {
        active_midi++;
      } else {
        active_midi = 61;
      }
    } else {
      if (active_pad > 0) {
        active_pad--;
      } else {
        active_pad = PAD_COUNT - 1;
      }
    }
  }
}