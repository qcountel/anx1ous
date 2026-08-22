#pragma once

#include "Module/Module.h"

namespace anx1ous::modules {

class Overlay final : public Module {
public:
    Overlay();
    std::string suffix() const override;

protected:
    void onEnable() override;
    void onDisable() override;
};

}
