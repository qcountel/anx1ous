#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class NoVSync final : public Module {
public:
    NoVSync();

protected:
    void onDisable() override;

private:
    void onRender(Render2DEvent& event);

    bool m_warned = false;
    bool m_hadVsync = true;
};

}
