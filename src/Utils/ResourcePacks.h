#pragma once

#include <string>

namespace anx1ous::packs {

struct SkyAssets {
    bool endSky = false;
    bool cubemapShader = false;
    bool faces = false;
    std::wstring facesDir;
};

SkyAssets scanActive();

}
