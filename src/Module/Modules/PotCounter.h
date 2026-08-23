#pragma once

#include <atomic>

#include "GUI/HudDrag.h"
#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
struct TickEvent;
}

namespace anx1ous::modules {

class PotCounter final : public Module {
public:
    PotCounter();

private:
    void onRender(Render2DEvent& event);
    void onTick(TickEvent& event);

    ColourSetting* m_colour;
    BoolSetting* m_rainbow;
    BoolSetting* m_background;
    FloatSetting* m_rounding;

    hud::Draggable m_drag;

    std::atomic<int> m_pots{0};
};

}
