#pragma once
#include "Synth.h"
#include <array>
#include <optional>

struct SynthCfg {
  Synth::Config synth;
  float amount = 1.0f;
};

class Pad {
public:
    struct Config {
        std::array<std::optional<SynthCfg>, 3> synths;
    };
    static const int MAX_SYNTHS = 3;

    Pad(float samplerate, const Config& config) : settings(config) {
        // WICHTIG: Nur i im Kopf hochzählen!
        for (int i = 0; i < MAX_SYNTHS; i++) {
            if (settings.synths[i].has_value()) {
                synths[i] = new Synth(samplerate, settings.synths[i].value().synth);
            } else {
                synths[i] = nullptr;
            }
        }
    }

    void SetGate(bool gate) {
        for (int i = 0; i < MAX_SYNTHS; i++) {
            if (synths[i] != nullptr) {
                synths[i]->SetGate(gate);
            }
        }
    }

    std::array<float, 2> Process() {
        float sigL = 0.0f;
        float sigR = 0.0f;
        for (int i = 0; i < MAX_SYNTHS; i++) {
            if (synths[i] != nullptr) {
                auto out = synths[i]->Process();
                sigL += out[0] * settings.synths[i].value().amount;
                sigR += out[1] * settings.synths[i].value().amount;
            }
        }
        return { sigL, sigR };
    }

private:
    Config settings;
    std::array<Synth*, MAX_SYNTHS> synths = {nullptr, nullptr, nullptr};
};