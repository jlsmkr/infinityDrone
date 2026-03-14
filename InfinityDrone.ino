#include "Synth.h"
#include "Pad.h"
#include "DaisyDuino.h"
#include "array"
#include "Filter.h"

float samplerate;
Synth::Config strings = {
  .keys = {
    {57, {1.0f, 1.009f}, Oscillator::WAVE_SAW, 0.3f},
    {64, {1.0f, 1.009f}, Oscillator::WAVE_SAW, 0.3f},
    {69, {1.0f, 1.009f}, Oscillator::WAVE_SAW, 0.1f},
  },
  .env_main = {5.0f, 0.1f, 1.0f, 1.6f},
  .filter   = {550.0f, 0.2f},
  .eq_lc    = {300.0f, 0.2f},
  .eq_bell  = {400.0f, 1.4f, -18.0f},
};
Synth::Config shimmer = {
  .keys = {
    {81, {1.0f, 1.003f}, Oscillator::WAVE_SQUARE, 0.1f},
    {88, {1.0f, 1.003f}, Oscillator::WAVE_SQUARE, 0.2f},
    {93, {1.0f, 1.003f}, Oscillator::WAVE_SQUARE, 0.2f},
    {100, {1.0f, 1.003f}, Oscillator::WAVE_SQUARE, 0.1f},
  },
  .env_main  = {20.0f, 0.1f, 1.0f, 1.6f},
  .filter   = {2000.0f, 0.2f},
  .eq_lc    = {500.0f, 0.2f},
  .eq_bell   = {550.0f, 1.0f, -3.0f},
};

Pad::Config pad1Cfg = {
  .synths = {{
    { SynthCfg{strings, 1.0f} },
    { SynthCfg{shimmer, 0.035f} },
    std::nullopt
  }}
};

static ReverbSc reverb;
static Pad* pad1 = nullptr;
Filter eq1;
Filter eq2;

Switch start_button;
Switch stop_button;

float start_button_pressed;
float stop_button_pressed;

void AudioCallback(float **in, float **out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float mixedL = 0.0f;
        float mixedR = 0.0f;

        if (pad1 != nullptr) {
            auto pOut = pad1->Process();
            mixedL = pOut[0];
            mixedR = pOut[1];
        }

        mixedL = eq1.Process(mixedL);
        mixedR = eq1.Process(mixedR);

        mixedL = eq2.Process(mixedL);
        mixedR = eq2.Process(mixedR);

        float revL, revR;
        reverb.Process(mixedL, mixedR, &revL, &revR);

        out[0][i] = mixedL * 0.2f + revL * 0.8f;
        out[1][i] = mixedR * 0.2f + revR * 0.8f;
    }
}

void setup() {
    start_button.Init(1000, true, 15, INPUT_PULLUP);
    stop_button.Init(1000, true, 16, INPUT_PULLUP);

    DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    float sr = DAISY.get_samplerate();

    reverb.Init(sr);
    reverb.SetFeedback(0.96f);
    reverb.SetLpFreq(16000.0f);
    eq1.Init(sr);
    eq1.SetPeak(4600.0f, 1.0f, 3.0f);
    eq2.Init(sr);
    eq2.SetPeak(500.0f, 1.2f, -7.0f);

    pad1 = new Pad(sr, pad1Cfg);

    DAISY.begin(AudioCallback);
}

void loop() {
  start_button.Debounce();
  stop_button.Debounce();

  if (pad1 != nullptr) {
    if (start_button.RisingEdge()) {
      pad1->SetGate(true);
    }
    if (stop_button.RisingEdge()) {
      pad1->SetGate(false);
    }
  }

  delay(1);
}