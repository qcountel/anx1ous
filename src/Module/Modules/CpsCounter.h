#pragma once

#include "GUI/HudDrag.h"
#include "GUI/Theme.h"
#include "Input/ClickCounter.h"
#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class CpsCounter final : public Module {
public:
    CpsCounter();

private:
    void onRender(Render2DEvent& event);

    EnumSetting* m_button;
    BoolSetting* m_background;
    BoolSetting* m_rainbow;
    ColourSetting* m_colour;
    FloatSetting* m_rounding;

    hud::Draggable m_drag;

    input::ClickCounter m_lmb, m_rmb;
};

}
