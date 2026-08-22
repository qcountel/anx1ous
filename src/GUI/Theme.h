#pragma once

#include <cmath>

#include "Utils/Math.h"

namespace anx1ous::gui {

struct Theme {
    static Theme& get();

    Colour background = Colour::rgb(0x000000, 0.65f);
    Colour panel      = Colour::rgb(0x0A0000, 0.75f);
    Colour panelAlt   = Colour::rgb(0x120000, 0.80f);
    Colour header     = Colour::rgb(0x0D0000, 0.78f);
    Colour outline    = Colour::rgb(0x3D0000, 0.25f);

    Colour accent     = Colour::rgb(0xDC143C);
    Colour accentAlt  = Colour::rgb(0xFF4444);

    Colour text       = Colour::rgb(0xD6DCE8);
    Colour textDim    = Colour::rgb(0x8A93A6);
    Colour textActive = Colour::rgb(0xFFFFFF);

    float cornerRadius = 10.0f;
    float animationSpeed = 0.20f;

    float rainbowSpeed = 40.0f;

    Colour rainbowAt(int index, float spread = 12.0f) const;

    Colour menuAccent(int index = 0) const;
};

float clockSeconds();

void advanceFrame();

float frameDelta();

class Animated {
public:
    Animated() = default;
    explicit Animated(float initial) : m_value(initial), m_target(initial) {}

    void set(float target) { m_target = target; }
    float target() const { return m_target; }

    float update(float speed) {
        if (speed >= 1.0f) {
            m_value = m_target;
            return m_value;
        }
        if (speed > 0.0f)
            m_value += (m_target - m_value) * (1.0f - std::pow(1.0f - speed, frameDelta() * 60.0f));

        if (std::fabs(m_target - m_value) < 0.001f)
            m_value = m_target;
        return m_value;
    }

    float value() const { return m_value; }

private:
    float m_value = 0.0f;
    float m_target = 0.0f;
};

}
