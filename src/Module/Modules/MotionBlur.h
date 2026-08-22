#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class MotionBlur final : public Module {
public:
    MotionBlur();

    std::string suffix() const override;

protected:
    void onEnable() override;
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);
    void push();

    FloatSetting* m_amount = nullptr;
    FloatSetting* m_opacity = nullptr;
};

}
