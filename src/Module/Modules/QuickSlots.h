#pragma once

#include "Module/Module.h"

namespace anx1ous {
struct KeyEvent;
}

namespace anx1ous::modules {

class QuickSlots final : public Module {
public:
    QuickSlots();

private:
    void onKey(KeyEvent& event);
};

}
