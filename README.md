# anx1ous

Внутренний клиент (DLL) для **Minecraft: Windows 10 Edition 1.1.5** с оверлей-интерфейсом в стиле Java-клиентов: ClickGui, HUD-модули и визуальные улучшения. Внедряется в процесс игры и отрисовывает интерфейс поверх кадра через Direct2D/D3D11.

> ⚠️ Проект нацелен строго на билд игры `1.1.5` (проверка по PE-заголовку при загрузке). На других версиях DLL не активируется.

---

## Возможности

### Visuals
Zoom, FreeLook, FullBright, Trajectories, Hitboxes, ItemPhysics, Skybox, MotionBlur,
SwingAnimations, ViewModel, JumpCircles, ChunkBorder, FogColor, HurtColor,
NoDynamicFov, NoHurtCam, NoRender, NoCamReset, SelfNameTag, AspectRatio, JavaFov

### Interface
ArrayList, Keystrokes, Mousestrokes, Watermark, CpsCounter, PotCounter, Notifications

### Input / Client
AutoSprint, DiagonalSprint, SensMultiplier, ItemDelayFix, KeybindFix, JavaHotkeys,
QuickSlots, Aliases, FastRefill, ClickGui, Overlay, NoVSync, PackSwitcher

Каждый модуль имеет настройки (слайдеры, переключатели, цвета, бинды), конфиги сохраняются в JSON и переключаются прямо из меню.

## Управление

| Клавиша | Действие |
|---|---|
| `Insert` | Открыть/закрыть меню |
| `End` | Выгрузить DLL из процесса |

## Сборка

**Требования:** Windows, Visual Studio 2022 с компонентом «C++ tools» (MSVC), CMake 3.21+. MinGW не поддерживается (ABI должна совпадать с игрой).

Самый простой путь:

```bat
build.bat          :: инкрементальная сборка Release
build.bat clean    :: пересборка с нуля
```

Готовая библиотека: `build\bin\Release\anx1ous.dll`.

Ручная сборка:

```bat
cmake -G "Visual Studio 17 2022" -A x64 -S . -B build
cmake --build build --config Release
```

Зависимости ([MinHook](https://github.com/TsudaKageyu/minhook), [nlohmann/json](https://github.com/nlohmann/json)) подтягиваются автоматически через FetchContent.

### Обфускация (опционально)

Сборка с OLLVM-обфускацией (нужен тулчейн [Arkari](https://github.com/kirbylover/arkari), путь задаётся переменной `ARKARI_ROOT`):

```bat
cmake --preset obf
cmake --build --preset obf
```

Hot-код (рендер и игровые хуки) не обфусцируется, чтобы не терять FPS.

## Архитектура

```
src/
├── Anx1ous.cpp      # ядро: startup/shutdown, проверка билда игры
├── dllmain.cpp      # точка входа, watchdog/scanner потоки, выгрузка по End
├── Hooks/           # MinHook-детуры: игра, ввод, FOV, чат, ресурсы, стабильность
├── Render/          # D2D/D3D11 оверлей, DrawUtils, MotionBlur, SkyCubemap
├── GUI/             # ClickGui, тема, перетаскиваемый HUD
├── Module/          # базовый класс модуля + ~37 модулей
├── Event/           # шина событий (Render2D, Tick, Key, Mouse, Packet...)
├── SDK/             # офсеты и обёртки ClientInstance/Level/Entity
├── Config/          # JSON-конфиги
├── Security/        # самопроверка целостности
└── Utils/           # логгер, память, патчи, crash-log
```

- **EventBus** — модули подписываются на события игры (`listen<Render2DEvent>(...)`)
- **Module** — декларативные настройки (`addFloat`, `addEnum`, `addColour`, ...)
- Офсеты захардкожены под конкретный билд и проверяются при старте по timestamp+размеру образа

## Дисклеймер

Проект создан в образовательных целях. Использование сторонних клиентов может нарушать правила серверов и условия использования игры. Ответственность за использование лежит на пользователе.
