#pragma once

#include <string>

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

namespace freelook {
bool active();
float cameraYaw();
float cameraPitch();
}

class FreeLook final : public Module {
public:
    FreeLook();

    std::string suffix() const override;
    bool holdBind() const override;

protected:
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    BoolSetting* m_hold;
    EnumSetting* m_mode;
};

}
