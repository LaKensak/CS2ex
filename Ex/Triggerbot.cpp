#include "Triggerbot.hpp"
#include "Offsets.hpp"
#include <Windows.h>

Triggerbot::Triggerbot(Memory& mem) : memory(mem), initialized(false) {}

bool Triggerbot::Initialize() {
    initialized = true;
    return true;
}

void Triggerbot::Run() {
    if (!initialized || !Config::triggerbot.enabled) return;
    if (Config::triggerbot.key && !GetAsyncKeyState(Config::triggerbot.key)) return;

    uintptr_t localPlayerPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwLocalPlayerController);
    if (!localPlayerPtr) return;

    Entity localPlayer(&memory, localPlayerPtr);
    if (!localPlayer.IsValid() || !localPlayer.IsAlive()) return;

    int crosshairId = localPlayer.GetCrosshairID();
    if (crosshairId <= 0 || crosshairId > 64) return;

    uintptr_t targetPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwEntityList + ((crosshairId - 1) * 0x10));
    if (!targetPtr) return;

    Entity target(&memory, targetPtr);
    if (!target.IsValid() || !target.IsAlive() || target.IsDormant() || !target.IsEnemy(localPlayer)) return;

    // Fire if ready
    uintptr_t weaponPtr = localPlayer.GetActiveWeapon();
    if (!weaponPtr) return;

    int shotsFired = localPlayer.GetShotsFired();

    // Simple delay implementation
    static DWORD lastShotTime = 0;
    DWORD currentTime = GetTickCount();
    if (currentTime - lastShotTime < (DWORD)Config::triggerbot.delay) return;

    // Shoot
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));

    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));

    lastShotTime = currentTime;
}
