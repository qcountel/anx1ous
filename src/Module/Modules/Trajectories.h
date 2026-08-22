#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct Render2DEvent;
}

namespace anx1ous::modules {

class Trajectories final : public Module {
public:
    Trajectories();

private:
    void onRender(Render2DEvent& event);

    ColourSetting* m_colour;
    FloatSetting* m_thickness;
    IntSetting* m_steps;
    BoolSetting* m_landing;
    BoolSetting* m_motion;
};

}
