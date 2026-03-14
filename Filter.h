#pragma once
#include <math.h>

class Filter
{
public:
    void Init(float samplerate)
    {
        sr_ = samplerate;
        z1_ = z2_ = 0.0f;
    }

    void SetPeak(float freq, float Q, float gainDB)
    {
        float A = powf(10.0f, gainDB / 40.0f);
        float w0 = 2.0f * M_PI * freq / sr_;
        float alpha = sinf(w0) / (2.0f * Q);
        float cosw0 = cosf(w0);

        float b0 = 1 + alpha * A;
        float b1 = -2 * cosw0;
        float b2 = 1 - alpha * A;
        float a0 = 1 + alpha / A;
        float a1 = -2 * cosw0;
        float a2 = 1 - alpha / A;

        b0_ = b0 / a0;
        b1_ = b1 / a0;
        b2_ = b2 / a0;
        a1_ = a1 / a0;
        a2_ = a2 / a0;
    }

    float Process(float x)
    {
        float y = b0_*x + z1_;
        z1_ = b1_*x - a1_*y + z2_;
        z2_ = b2_*x - a2_*y;
        return y;
    }

private:
    float sr_;
    float b0_, b1_, b2_, a1_, a2_;
    float z1_, z2_;
};