#include "Anx1ous.h"

#include <Windows.h>
#include <Psapi.h>
#include <filesystem>

#include "Config/Config.h"
#include "GUI/ClickGui.h"
#include "Hooks/Hooks.h"
#include "Input/InputManager.h"
#include "Module/ModuleManager.h"
#include "Render/Overlay.h"
#include "Render/DrawUtils.h"
#include "Render/MotionBlur.h"
#include "SDK/Context.h"
#include "Security/Scanner.h"
#include "Utils/CrashLog.h"
#include "Utils/Hook.h"
#include "Utils/Logger.h"
#include "Utils/Memory.h"
#include "Utils/Platform.h"

namespace anx1ous {
namespace {

constexpr uint32_t kExpectedTimestamp = 0x5976DA7B;
constexpr uint32_t kExpectedImageSize = 0x1A91000;

struct BuildInfo {
    uint32_t timestamp = 0;
    uint32_t imageSize = 0;
};

BuildInfo readBuildInfo() {
    BuildInfo info;
    const auto base = memory::base();
    if (!base)
        return info;

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return info;

    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return info;

    info.timestamp = nt->FileHeader.TimeDateStamp;
    info.imageSize = nt->OptionalHeader.SizeOfImage;
    return info;
}

}

Anx1ous& Anx1ous::get() {
    static Anx1ous instance;
    return instance;
}

bool Anx1ous::verifyGameBuild() {
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, MAX_PATH);

    const BuildInfo info = readBuildInfo();
    LOG_INFO("anx1ous", "host: {} (base {:#x}, image {:#x}, timestamp {:#x})",
             std::filesystem::path(path).filename().string(), memory::base(), info.imageSize,
             info.timestamp);

    if (info.timestamp != kExpectedTimestamp || info.imageSize != kExpectedImageSize) {
        LOG_ERROR("anx1ous",
                  "build mismatch: expected timestamp {:#x} / image {:#x}, got {:#x} / {:#x}",
                  kExpectedTimestamp, kExpectedImageSize, info.timestamp, info.imageSize);
        return false;
    }
    return true;
}

void Anx1ous::startup(void* moduleHandle) {
    m_module = moduleHandle;

    Logger::get().init();
    LOG_INFO("anx1ous", "anx1ous " ANX1OUS_VERSION " starting");

    crash::install();

    const bool buildMatches = verifyGameBuild();
    if (!buildMatches) {

        LOG_ERROR("anx1ous", "aborting startup - wrong game build");
        MessageBoxW(nullptr,
                    L"anx1ous targets Minecraft: Windows 10 Edition 1.1.5.\n"
                    L"The running game is a different build, so the client will not load.",
                    L"anx1ous", MB_ICONERROR | MB_OK);
        crash::remove();
        Logger::get().shutdown();
        return;
    }

    ModuleManager::get().registerAll();

    if (!hooks::installAll()) {

        LOG_ERROR("anx1ous", "hook installation failed - rolling back");

        input::InputManager::get().removeMessageHook();
        hooks::removeAll();
        render::DrawUtils::releaseResources();
        render::MotionBlur::get().shutdown();
        render::Overlay::get().shutdown();
        ModuleManager::get().shutdown();

        crash::remove();
        Logger::get().shutdown();
        return;
    }

    Config::get().loadActive();

    security::Scanner::get().init(m_module);

    platform::holdExecution();

    m_running = true;
    LOG_INFO("anx1ous", "ready - press F12 for the menu, End to unload");
}

void Anx1ous::shutdown() {
    if (!m_running.exchange(false))
        return;

    LOG_INFO("anx1ous", "shutting down");

    Config::get().save();

    LOG_DEBUG("anx1ous", "teardown: menu");
    if (!hooks::requestTeardown(500))
        gui::ClickGui::get().close();

    LOG_DEBUG("anx1ous", "teardown: message hook");
    input::InputManager::get().removeMessageHook();

    LOG_DEBUG("anx1ous", "teardown: detours");
    hooks::removeAll();

    LOG_DEBUG("anx1ous", "teardown: overlay");
    render::DrawUtils::releaseResources();
    render::MotionBlur::get().shutdown();
    render::Overlay::get().shutdown();

    LOG_DEBUG("anx1ous", "teardown: modules");
    platform::releaseExecution();
    platform::detachFromGameInput();
    ModuleManager::get().shutdown();
    sdk::Context::get().reset();

    LOG_INFO("anx1ous", "unloaded");

    crash::remove();
    Logger::get().shutdown();
}

}
