#include "Module/Modules/AutoSprint.h"

#include "Utils/Obfusc.h"

namespace anx1ous::modules {

AutoSprint::AutoSprint()
    : PatchModule("AutoSprint", "Always sprints", Category::Input,
                  BytePatch(ANX1OUS_STR("0F B6 41 ? 40 32 ED"),
                            {0xB0, 0x01,
                             0x90, 0x90,
                             0x40, 0x32, 0xED})) {}

}
