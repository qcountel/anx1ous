#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class FogColor final : public Module {
public:
    FogColor();

protected:
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    EnumSetting* m_preset;
    ColourSetting* m_custom;
    BoolSetting* m_rainbow;
    FloatSetting* m_rainbowSpeed;
};

}
