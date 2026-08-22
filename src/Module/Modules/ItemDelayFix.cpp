#include "Module/Modules/ItemDelayFix.h"

#include "Utils/Obfusc.h"

namespace anx1ous::modules {

ItemDelayFix::ItemDelayFix()
    : PatchModule("ItemDelayFix", "Removes the 200 ms delay after attacking", Category::Input,
                  BytePatch::nops(ANX1OUS_STR("48 89 86 ? ? ? ? 48 83 7E ? 00"), 7)) {}

}
