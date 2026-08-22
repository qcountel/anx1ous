#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class JavaFov final : public Module {
public:
    JavaFov();

protected:
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    FloatSetting* m_sprintFov;
    FloatSetting* m_smoothing;

    float m_current = 1.0f;
};

}
