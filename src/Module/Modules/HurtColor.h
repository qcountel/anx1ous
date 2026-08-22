#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class HurtColor final : public Module {
public:
    HurtColor();

protected:
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    ColourSetting* m_colour;
};

}
