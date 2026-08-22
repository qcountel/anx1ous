#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class FastRefill final : public Module {
public:
    FastRefill();

protected:
    void onEnable() override;
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    BoolSetting* m_requireShift;
};

}
