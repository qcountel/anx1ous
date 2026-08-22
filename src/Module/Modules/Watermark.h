#pragma once

#include "GUI/HudDrag.h"
#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class Watermark final : public Module {
public:
    Watermark();

    bool listed() const override { return false; }

private:
    void onRender(Render2DEvent& event);

    EnumSetting* m_style;
    BoolSetting* m_showFps;
    BoolSetting* m_rainbow;
    ColourSetting* m_colour;

    FloatSetting* m_posX;
    FloatSetting* m_posY;
    hud::Draggable m_drag;
};

}
