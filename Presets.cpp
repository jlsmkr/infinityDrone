#include "Presets.h"

// ---------- SYNTHS ---------- //

// 1. Deep Sea

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

// unused
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

// 2. Forest
const Synth::Config verby_sub DSY_SDRAM_BSS = {
  .notes = {
    {-12, 1.001f, Oscillator::WAVE_TRI, 1.0f}, // A2
    {-5, 1.001f, Oscillator::WAVE_TRI, 1.0f}, // E3
  },
  .active_keys_count = 2,
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .eq_lpf    = {800.0f, 0.2f},
  .eq_hpf    = {400.0f, 0.2f},
  .eq_bell1   = {300.0f, 0.3f, -16.0f},
  .eq_bell2   = {1800.0f, 1.0f, 0.0f},
  .lfo_amp  = {0.1f, 0.05f, true},
  .lfo_lpf  = {0.0f, 0.0f, false},
};

const Synth::Config verby_mid DSY_SDRAM_BSS = {
  .notes = {
    {-5, 1.001f, Oscillator::WAVE_SQUARE, 1.0f}, // A2
    {-2, 1.001f, Oscillator::WAVE_SQUARE, 1.0f}, // E3
  },
  .active_keys_count = 2,
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .eq_lpf    = {2000.0f, 0.2f},
  .eq_hpf    = {1000.0f, 0.2f},
  .eq_bell1   = {1000.0f, 1.0f, -20.0f},
  .eq_bell2   = {2000.0f, 2.0f, -15.0f},
  .lfo_amp  = {0.8f, 0.05f, true},
  .lfo_lpf  = {0.0f, 0.0f, false},
};

// ----------- PADS ----------- //

// like ocean or something
Pad::Config deep = {
  .layers = {
    {deep_sub, 0.4f},
    {deep_warm, 0.5f},
    {deep_mid, 0.1f},
  },
  .active_layers_count = 3,
  .eq_bell1 = {300.0f, 1.0f, -0.7f},
  .eq_bell2 = {800.0f, 1.5f, -4.0f},
  .name = "DEEP",
};

// like forest or something
Pad::Config verby = {
  .layers = {
    {verby_sub, 0.4f},
    {verby_mid, 0.4f},
  },
  .active_layers_count = 2,
  .eq_bell1 = {300.0f, 1.0f, -0.0f},
  .eq_bell2 = {800.0f, 1.5f, -0.0f},
  .name = "VRBY",
};

// bright, thin, no deep notes
Pad::Config airy;