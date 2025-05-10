#pragma once
#include "Memory.hpp"
#include "Entity.hpp"
#include "Vector.hpp"
#include "Config.hpp"
#include <cmath>

class Aimbot {
private:
    Memory& memory;
    bool initialized;

public:
    Aimbot(Memory& mem);

    bool Initialize();
    void Run();

private:
    Vector3 CalculateAngle(const Vector3& src, const Vector3& dst);
    float CalculateFov(const Vector3& viewAngle, const Vector3& aimAngle);
    void NormalizeAngles(Vector3& angle);
    Vector3 SmoothAngle(const Vector3& src, const Vector3& dst, float factor);
    Vector3 GetViewAngles();
    void SetViewAngles(const Vector3& angles);
};
