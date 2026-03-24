#include "DaisyDuino.h"
#include "Synth.h"
#include "Pad.h"

#define PAD_COUNT 2

// PAD CFG

Synth::Config strings PROGMEM = {
  .notes = {
    {-12, 1.009f, Oscillator::WAVE_SAW, 0.3f}, // A3 57
    {-5, 1.009f, Oscillator::WAVE_SAW, 0.3f}, // E4 64
    {0, 1.009f, Oscillator::WAVE_SAW, 0.1f}, // A4 69
    {0, 1.009f, Oscillator::WAVE_SAW, 0.0f},
  },
  .active_keys_count = 2,
  .env_main = {5.0f, 0.1f, 1.0f, 1.6f},
  .eq_hc   = {550.0f, 0.2f},
  .eq_lc    = {300.0f, 0.2f},
  .eq_bell  = {400.0f, 1.4f, -18.0f},
};

Synth::Config shimmer PROGMEM = {
  .notes = {
    {12, 1.003f, Oscillator::WAVE_SQUARE, 0.1f}, // A5 81
    {19, 1.003f, Oscillator::WAVE_SQUARE, 0.2f}, // E6 88
    {24, 1.003f, Oscillator::WAVE_SQUARE, 0.2f}, // A6 93
    {31, 1.003f, Oscillator::WAVE_SQUARE, 0.1f}, // E7 100
  },
  .active_keys_count = 2,
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .eq_hc   = {2000.0f, 0.2f},
  .eq_lc    = {500.0f, 0.2f},
  .eq_bell   = {550.0f, 1.0f, -3.0f},
};

Pad::Config pad1Cfg PROGMEM = {
  .layers = {
    {strings, 1.0f},
  },
  .active_layers_count = 1,
};

Pad::Config pad2Cfg PROGMEM = {
  .layers = {
    {shimmer, 1.0f},
  },
  .active_layers_count = 1,
};

// MAIN

float sr;
int active_midi = 69; // 404 Hz
bool active_synth = false;
bool set_type = true; // true: midi; false: pad
int active_pad = 0;

Pad pad1, pad2;
static ReverbSc reverb;

Switch start_button, stop_button, set_button, prev_button, next_button;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float l = 0.0f;
    float r = 0.0f;

    pad1.Process(l, r);
    pad2.Process(l, r);

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
    pad2.SetGate(false);
    pad1.ApplyConfig(cfg, active_midi);
    pad1.SetGate(true);
  } else {
    // no 2
    pad1.SetGate(false);
    pad2.ApplyConfig(cfg, active_midi);
    pad2.SetGate(true);
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

  pad1.Init(sr);
  pad2.Init(sr);

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
    pad1.SetGate(false);
    pad2.SetGate(false);
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