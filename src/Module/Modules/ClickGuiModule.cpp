#include "Module/Modules/ClickGuiModule.h"

#include <Windows.h>

#include "Event/Events.h"
#include "GUI/CharacterArt.h"
#include "GUI/ClickGui.h"

namespace anx1ous::modules {
namespace {

std::vector<std::string> characterOptions() {
    std::vector<std::string> options{"None"};
    for (const auto& art : gui::kCharacterArt)
        options.emplace_back(art.label);
    return options;
}

}

ClickGuiModule::ClickGuiModule()
    : Module("ClickGui", "Opens the settings interface", Category::Client, VK_F12) {
    m_character = addEnum("Character", "Artwork behind the module list", characterOptions(), 1);
    m_characterOpacity = addFloat("Character opacity", "How strongly the artwork shows", 0.40f, 0.1f,
                                  1.0f, 0.05f);
    m_characterOpacity->onlyIf([this] { return !m_character->is("None"); });
    m_title = addText("Title", "Name shown in the menu header", "anx1ous");

    listenAlways<Render2DEvent>(&ClickGuiModule::onRender, kPriorityHighest);
    listenAlways<MouseEvent>(&ClickGuiModule::onMouse, kPriorityHighest);
    listenAlways<KeyEvent>(&ClickGuiModule::onKey, kPriorityHighest);
    listenAlways<CharEvent>(&ClickGuiModule::onChar, kPriorityHighest);
}

void ClickGuiModule::onEnable() { gui::ClickGui::get().open(); }

void ClickGuiModule::onDisable() { gui::ClickGui::get().close(); }

void ClickGuiModule::onRender(Render2DEvent& event) {

    if (enabled() && !gui::ClickGui::get().isOpen())
        setEnabled(false);

    gui::ClickGui::get().render(event);
}

void ClickGuiModule::onMouse(MouseEvent& event) { gui::ClickGui::get().onMouse(event); }

void ClickGuiModule::onKey(KeyEvent& event) { gui::ClickGui::get().onKey(event); }

void ClickGuiModule::onChar(CharEvent& event) { gui::ClickGui::get().onChar(event); }

}
