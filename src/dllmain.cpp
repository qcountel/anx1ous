#include <Windows.h>

#include <atomic>
#include <cstdint>

#include "Anx1ous.h"
#include "Hooks/Hooks.h"
#include "Input/InputManager.h"
#include "Render/Overlay.h"
#include "Render/DrawUtils.h"
#include "Security/Scanner.h"
#include "Utils/Logger.h"
#include "Utils/Platform.h"

namespace {

HMODULE g_module = nullptr;

void logHeartbeat() {
    static uint64_t lastFrames = 0;
    static uint64_t lastTicks = 0;
    static uint64_t lastUpdates = 0;
    static uint64_t lastOverlays = 0;

    static anx1ous::input::InputManager::Stats lastInput;

    const uint64_t frames = anx1ous::hooks::renderFrameCount();
    const uint64_t ticks = anx1ous::hooks::playerTickCount();
    const uint64_t updates = anx1ous::hooks::gameUpdateCount();
    const uint64_t overlays = anx1ous::hooks::overlayCount();
    const auto input = anx1ous::input::InputManager::get().stats();
    const auto draw = anx1ous::render::DrawUtils::stats();
    const auto foreground = anx1ous::platform::foregroundInfo();

    LOG_INFO("Watchdog",
             "updates +{} frames +{} ticks +{} overlays +{} | samples +{} polls +{} keys +{} last '{}' | "
             "down async={} sync={} | msgs +{} wheels +{} last pointer msg 0x{:04X} | "
             "fills {}/{} texts {}/{} | backend {} ({}) | focused={} | fg {} pid {} class '{}'",
             updates - lastUpdates, frames - lastFrames, ticks - lastTicks, overlays - lastOverlays,
             input.samples - lastInput.samples, input.polls - lastInput.polls,
             input.transitions - lastInput.transitions,
             anx1ous::input::InputManager::keyName(input.lastKey), input.asyncDowns, input.syncDowns,
             input.hookCalls - lastInput.hookCalls, input.wheelEvents - lastInput.wheelEvents,
             input.lastPointerMessage,
             draw.fills, draw.fillsSkipped, draw.texts, draw.textsSkipped,
             anx1ous::render::DrawUtils::backendName(), anx1ous::render::Overlay::get().status(),
             anx1ous::platform::gameFocused(), static_cast<void*>(foreground.window),
             foreground.processId, foreground.className);

    lastFrames = frames;
    lastTicks = ticks;
    lastUpdates = updates;
    lastOverlays = overlays;
    lastInput = input;
}

HMODULE loaderModule() {
    HMODULE module = nullptr;
    const bool found = GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                          reinterpret_cast<LPCWSTR>(&loaderModule), &module);
    return found ? module : nullptr;
}

DWORD WINAPI startupThread(LPVOID) {
    anx1ous::Anx1ous::get().startup(g_module);

    LOG_INFO("anx1ous", "loaded from {} ({})", static_cast<void*>(loaderModule()),
             loaderModule() ? "loader-mapped" : "manually mapped - eject will not unload the image");

    constexpr int kIntervalMs = 15;
    constexpr int kHeartbeatTicks = 3000 / kIntervalMs;

    int sinceHeartbeat = 0;
    while (!anx1ous::Anx1ous::get().shutdownRequested()) {
        if (GetAsyncKeyState(VK_END) & 0x8000)
            break;

        if (anx1ous::Anx1ous::get().running()) {
            anx1ous::input::InputManager::get().sample();

            if (++sinceHeartbeat >= kHeartbeatTicks) {
                sinceHeartbeat = 0;
                logHeartbeat();
            }
        }
        Sleep(kIntervalMs);
    }

    anx1ous::Anx1ous::get().shutdown();

    HMODULE self = loaderModule();
    if (self)
        FreeLibraryAndExitThread(self, 0);

    ExitThread(0);
}

std::atomic<uint64_t> g_watchdogBeat{0};

DWORD WINAPI scannerThread(LPVOID);
DWORD WINAPI watchdogThread(LPVOID);

void takeSelfRef(HMODULE* out) {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                       reinterpret_cast<LPCWSTR>(&takeSelfRef), out);
}

DWORD WINAPI scannerThread(LPVOID) {
    HMODULE selfRef = nullptr;
    takeSelfRef(&selfRef);

    uint64_t lastWatch = 0;
    int watchStale = 0;
    while (!anx1ous::Anx1ous::get().shutdownRequested()) {
        if (GetAsyncKeyState(VK_END) & 0x8000)
            break;

        if (anx1ous::Anx1ous::get().running()) {
            anx1ous::security::Scanner::get().scan();

            const uint64_t beat = g_watchdogBeat.load(std::memory_order_relaxed);
            if (beat == lastWatch) {
                if (++watchStale >= 4 && !anx1ous::Anx1ous::get().shutdownRequested()) {
                    watchStale = 0;
                    const HANDLE h = CreateThread(nullptr, 0, watchdogThread, nullptr, 0, nullptr);
                    if (h)
                        CloseHandle(h);
                }
            } else {
                lastWatch = beat;
                watchStale = 0;
            }
        }
        Sleep(2000);
    }

    if (selfRef)
        FreeLibraryAndExitThread(selfRef, 0);
    ExitThread(0);
}

DWORD WINAPI watchdogThread(LPVOID) {
    HMODULE selfRef = nullptr;
    takeSelfRef(&selfRef);

    uint64_t lastScan = 0;
    int scanStale = 0;
    while (!anx1ous::Anx1ous::get().shutdownRequested()) {
        if (GetAsyncKeyState(VK_END) & 0x8000)
            break;

        g_watchdogBeat.fetch_add(1, std::memory_order_relaxed);

        if (anx1ous::Anx1ous::get().running()) {
            const uint64_t beat = anx1ous::security::Scanner::get().heartbeat();
            if (beat == lastScan) {
                if (++scanStale >= 6) {
                    scanStale = 0;
                    anx1ous::security::Scanner::get().scan();
                    if (!anx1ous::Anx1ous::get().shutdownRequested()) {
                        const HANDLE h =
                            CreateThread(nullptr, 0, scannerThread, nullptr, 0, nullptr);
                        if (h)
                            CloseHandle(h);
                    }
                }
            } else {
                lastScan = beat;
                scanStale = 0;
            }
        }
        Sleep(2000);
    }

    if (selfRef)
        FreeLibraryAndExitThread(selfRef, 0);
    ExitThread(0);
}

}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    switch (reason) {
    case DLL_PROCESS_ATTACH: {
        g_module = module;
        DisableThreadLibraryCalls(module);

        const HANDLE thread = CreateThread(nullptr, 0, startupThread, nullptr, 0, nullptr);
        if (thread)
            CloseHandle(thread);

        const HANDLE scanner = CreateThread(nullptr, 0, scannerThread, nullptr, 0, nullptr);
        if (scanner)
            CloseHandle(scanner);

        const HANDLE watchdog = CreateThread(nullptr, 0, watchdogThread, nullptr, 0, nullptr);
        if (watchdog)
            CloseHandle(watchdog);
        break;
    }
    case DLL_PROCESS_DETACH:

        break;
    default:
        break;
    }
    return TRUE;
}
