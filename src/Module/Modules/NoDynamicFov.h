#pragma once

#include "Module/Module.h"

namespace anx1ous::modules {

class NoDynamicFov final : public Module {
public:
    NoDynamicFov();

protected:
    void onEnable() override;
    void onDisable() override;
};

}
