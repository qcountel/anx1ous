#pragma once

#include "Assets/Resources.h"

namespace anx1ous::gui {

// Single source of truth for the ClickGui background artwork picker.
//
// To add a new character:
//   1. Drop the PNG into assets/
//   2. Add an RCDATA line for it in src/Assets/Resources.rc
//   3. Add an ANX1OUS_ASSET_* id for it in src/Assets/Resources.h
//   4. Add a row to kCharacterArt below
//
// The "Character" enum setting (ClickGuiModule) and the artwork lookup
// (ClickGui::activeCharacter) both build themselves from this table, so
// nothing else needs to change.
struct CharacterArt {
    const char* label;
    int resourceId;
};

inline constexpr CharacterArt kCharacterArt[] = {
    {"Rei", ANX1OUS_ASSET_AYANAMI},
    {"Asuka", ANX1OUS_ASSET_ASUKA},
    {"Rem", ANX1OUS_ASSET_REM},
    {"Yuki", ANX1OUS_ASSET_YUKI},
};

}
