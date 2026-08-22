#pragma once

namespace anx1ous::hooks {

struct Installer {
    Installer(const char* name, bool (*install)());
};

}
