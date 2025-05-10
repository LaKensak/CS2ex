#include "Aimbot.hpp"
#include "Offsets.hpp"
#include <Windows.h>

Aimbot::Aimbot(Memory& mem) : memory(mem), initialized(false) {}

bool Aimbot::Initialize() {
    initialized = true;
    return true;
}

void Aimbot::Run() {
    if (!initialized || !Config::aimbot.enabled) return;
    if (!GetAsyncKeyState(Config::aimbot.key)) return;

    uintptr_t localPlayerPtr = memory.Read<uintptr_t>(
        memory.GetClientBase() + offsets::signatures::dwLocalPlayerController
    );

    if (!localPlayerPtr) return;

    Entity localPlayer(&memory, localPlayerPtr);
    if (!localPlayer.IsValid() || !localPlayer.IsAlive()) return;

    Vector3 localEyePos = localPlayer.GetEyePosition();
    Vector3 viewAngles = GetViewAngles();
    Vector3 aimPunch = localPlayer.GetAimPunchAngle() * 2.0f;

    float bestFov = Config::aimbot.fov;
    Vector3 bestAngle;
    bool targetFound = false;

    for (int i = 1; i < 64; i++) {
        uintptr_t entityPtr = memory.Read<uintptr_t>(
            memory.GetClientBase() + offsets::signatures::dwEntityList + (i * 0x10)
        );

        if (!entityPtr) continue;

        Entity entity(&memory, entityPtr);
        if (!entity.IsValid() || !entity.IsAlive() || entity.IsDormant() || !entity.IsEnemy(localPlayer)) continue;

        Vector3 targetPos;
        switch (Config::aimbot.bone) {
        case 0: targetPos = entity.GetBonePosition(8); break;
        case 1: targetPos = entity.GetBonePosition(7); break;
        case 2: targetPos = entity.GetBonePosition(6); break;
        case 3: targetPos = entity.GetBonePosition(4); break;
        default: targetPos = entity.GetBonePosition(8); break;
        }

        Vector3 angle = CalculateAngle(localEyePos, targetPos);

        float fov = CalculateFov(viewAngles, angle);

        if (fov < bestFov) {
            bestFov = fov;
            bestAngle = angle;
            targetFound = true;
        }
    }

    if (targetFound) {
        if (Config::aimbot.smooth > 1.0f) {
            bestAngle = SmoothAngle(viewAngles, bestAngle, Config::aimbot.smooth);
        }

        NormalizeAngles(bestAngle);
        SetViewAngles(bestAngle);
    }
}

Vector3 Aimbot::CalculateAngle(const Vector3& src, const Vector3& dst) {
    Vector3 angle;
    Vector3 delta = dst - src;

    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

    angle.x = atanf(delta.z / hyp) * 57.295779513082f;
    angle.y = atanf(delta.y / delta.x) * 57.295779513082f;
    angle.z = 0.0f;

    if (delta.x >= 0.0f) {
        angle.y += 180.0f;
    }

    return angle;
}

float Aimbot::CalculateFov(const Vector3& viewAngle, const Vector3& aimAngle) {
    Vector3 delta = aimAngle - viewAngle;
    NormalizeAngles(delta);

    return sqrt(delta.x * delta.x + delta.y * delta.y);
}

void Aimbot::NormalizeAngles(Vector3& angle) {
    while (angle.x > 89.0f) angle.x -= 180.0f;
    while (angle.x < -89.0f) angle.x += 180.0f;

    while (angle.y > 180.0f) angle.y -= 360.0f;
    while (angle.y < -180.0f) angle.y += 360.0f;

    angle.z = 0.0f;
}

Vector3 Aimbot::SmoothAngle(const Vector3& src, const Vector3& dst, float factor) {
    Vector3 delta = dst - src;
    NormalizeAngles(delta);

    return src + delta / factor;
}

Vector3 Aimbot::GetViewAngles() {
    return memory.Read<Vector3>(memory.GetClientBase() + offsets::signatures::dwViewAngles);
}

void Aimbot::SetViewAngles(const Vector3& angles) {
    memory.Write<Vector3>(memory.GetClientBase() + offsets::signatures::dwViewAngles, angles);
}
