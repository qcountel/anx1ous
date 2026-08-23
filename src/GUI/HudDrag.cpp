#include "GUI/HudDrag.h"

#include <Windows.h>

#include <algorithm>
#include <string>

#include "Input/InputManager.h"
#include "Module/Setting.h"
#include "SDK/Context.h"

namespace anx1ous::hud {
namespace {

bool cursorFree() {
    if (!input::InputManager::gameFocused())
        return false;
    const std::string screen = sdk::Context::get().currentScreenName();
    return !screen.empty() && screen != "hud_screen" && screen != "start_screen";
}

std::vector<Element>& registry() {
    static std::vector<Element> list;
    return list;
}

bool g_editorActive = false;

}

Vec2 Draggable::place(Vec2 size, Vec2 screen) {
    if (!m_x || !m_y || screen.x <= 0.0f || screen.y <= 0.0f)
        return {0.0f, 0.0f};

    if (m_x->value < 0.0f || m_y->value < 0.0f) {
        const float margin = 20.0f;
        m_x->value = std::clamp((screen.x - size.x - margin) / screen.x, 0.0f, 1.0f);
        m_y->value = std::clamp(margin / screen.y, 0.0f, 1.0f);
    }

    Vec2 pos{m_x->value * screen.x, m_y->value * screen.y};

    // The HUD editor drives position with moveTo() while it is open; the
    // in-game right-click drag would otherwise fight over the same setting.
    if (!g_editorActive && input::InputManager::get().isDown(VK_RBUTTON) && cursorFree()) {
        const Vec2 cur = input::InputManager::get().cursor();
        const bool inside = cur.x >= pos.x && cur.x <= pos.x + size.x && cur.y >= pos.y &&
                            cur.y <= pos.y + size.y;
        if (!m_dragging && inside) {
            m_dragging = true;
            m_grab = {cur.x - pos.x, cur.y - pos.y};
        }
        if (m_dragging)
            pos = {cur.x - m_grab.x, cur.y - m_grab.y};
    } else {
        m_dragging = false;
    }

    pos.x = std::clamp(pos.x, 0.0f, std::max(0.0f, screen.x - size.x));
    pos.y = std::clamp(pos.y, 0.0f, std::max(0.0f, screen.y - size.y));

    m_x->value = pos.x / screen.x;
    m_y->value = pos.y / screen.y;

    m_bounds = {pos.x, pos.y, pos.x + size.x, pos.y + size.y};
    m_placed = true;
    return pos;
}

void Draggable::moveTo(Vec2 position, Vec2 size, Vec2 screen) {
    if (!m_x || !m_y || screen.x <= 0.0f || screen.y <= 0.0f)
        return;

    position.x = std::clamp(position.x, 0.0f, std::max(0.0f, screen.x - size.x));
    position.y = std::clamp(position.y, 0.0f, std::max(0.0f, screen.y - size.y));

    m_x->value = position.x / screen.x;
    m_y->value = position.y / screen.y;

    m_bounds = {position.x, position.y, position.x + size.x, position.y + size.y};
}

void registerElement(Module* module, Draggable* drag) {
    auto& list = registry();
    for (auto& entry : list) {
        if (entry.module == module) {
            entry.drag = drag;
            return;
        }
    }
    list.push_back({module, drag});
}

void unregisterElement(Module* module) {
    auto& list = registry();
    list.erase(std::remove_if(list.begin(), list.end(),
                              [module](const Element& e) { return e.module == module; }),
               list.end());
}

const std::vector<Element>& elements() { return registry(); }

void setEditorActive(bool active) { g_editorActive = active; }
bool editorActive() { return g_editorActive; }

}
