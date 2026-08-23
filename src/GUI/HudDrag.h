#pragma once

#include <string>
#include <vector>

#include "Utils/Math.h"

namespace anx1ous {

class FloatSetting;
class Module;

namespace hud {

class Draggable {
public:
    void bind(FloatSetting* x, FloatSetting* y) {
        m_x = x;
        m_y = y;
    }

    Vec2 place(Vec2 size, Vec2 screen);

    bool dragging() const { return m_dragging; }

    // Last rectangle this element occupied on screen, in pixels. The HUD editor
    // uses it for hit-testing and for drawing the selection frame, so it stays
    // valid even on frames where the element is not being dragged.
    Rect bounds() const { return m_bounds; }
    bool placed() const { return m_placed; }

    // Moves the element to an absolute screen position, clamped to the screen.
    // The editor drives this instead of the in-game right-click drag.
    void moveTo(Vec2 position, Vec2 size, Vec2 screen);

private:
    FloatSetting* m_x = nullptr;
    FloatSetting* m_y = nullptr;
    bool m_dragging = false;
    Vec2 m_grab{};

    Rect m_bounds{};
    bool m_placed = false;
};

// Registry of every draggable HUD element, so the editor can enumerate them
// without each module having to know the editor exists. Modules register on
// construction through `Module::addHudPosition`.
struct Element {
    Module* module = nullptr;
    Draggable* drag = nullptr;
};

void registerElement(Module* module, Draggable* drag);
void unregisterElement(Module* module);
const std::vector<Element>& elements();

// While the editor owns the cursor the in-game right-click drag must stay off,
// otherwise both would fight over the same position settings.
void setEditorActive(bool active);
bool editorActive();

}
}
