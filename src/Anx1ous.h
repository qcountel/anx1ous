#pragma once

#include <atomic>

namespace anx1ous {

class Anx1ous {
public:
    static Anx1ous& get();

    void startup(void* moduleHandle);
    void shutdown();

    bool running() const { return m_running; }
    void requestShutdown() { m_shutdownRequested = true; }
    bool shutdownRequested() const { return m_shutdownRequested; }

    void* moduleHandle() const { return m_module; }

private:
    Anx1ous() = default;

    bool verifyGameBuild();

    void* m_module = nullptr;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_shutdownRequested{false};
};

}
