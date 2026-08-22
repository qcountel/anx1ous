#include "Module/Modules/NoHurtCam.h"

#include "Utils/Obfusc.h"

namespace anx1ous::modules {

NoHurtCam::NoHurtCam()
    : PatchModule("NoHurtCam", "Stops the camera shaking when you take damage", Category::Visuals,
                  BytePatch::nops(ANX1OUS_STR("66 44 0F 6E 83 ? ? ? ? 45 0F 5B C0 44 0F 29 4C 24"), 9)) {}

}
