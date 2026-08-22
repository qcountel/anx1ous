#pragma once

#include <string>

#include "Module/Module.h"
#include "Utils/Patch.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class SelfNameTag final : public Module {
public:
    SelfNameTag();

    std::string suffix() const override;

protected:
    void onEnable() override;
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    BytePatch m_patch;

    BoolSetting* m_frontView;
    BoolSetting* m_ignoreColour;
    BoolSetting* m_useText;
    ColourSetting* m_textColour;
    BoolSetting* m_useBg;
    ColourSetting* m_bgColour;
    FloatSetting* m_scale;
};

}
