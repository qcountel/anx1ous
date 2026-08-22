#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class SensMultiplier final : public Module {
public:
    SensMultiplier();

protected:
    void onEnable() override;
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    FloatSetting* m_multiplier;
    BoolSetting* m_mouseKeyboard;
    BoolSetting* m_controller;
    BoolSetting* m_touch;
};

}
