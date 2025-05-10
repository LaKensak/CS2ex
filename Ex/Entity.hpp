#pragma once
#include "Memory.hpp"
#include "Vector.hpp"

class Entity {
private:
    Memory* m_memory;
    DWORD m_entityAddress;

public:
    Entity(Memory* memory, DWORD entityAddress);

    bool IsValid() const;
    int GetHealth() const;
    int GetTeam() const;
    bool IsDormant() const;
    Vector3 GetOrigin() const;
    Vector3 GetEyePosition() const;
    Vector3 GetViewOffset() const;
    Vector3 GetBonePosition(int boneID) const;
    bool IsAlive() const;
    bool IsEnemy(const Entity& localPlayer) const;
    Vector3 GetAimPunchAngle() const;
    bool IsDefusing() const;
    float GetFlashDuration() const;
    int GetShotsFired() const;
    int GetCrosshairID() const;
    bool IsScoped() const;
    int GetFlags() const;
    bool IsOnGround() const;
    uintptr_t GetActiveWeapon() const;
    int GetWeaponID() const;
};