#include "Entity.hpp"
#include "Offsets.hpp"

Entity::Entity(Memory* memory, DWORD entityAddress)
    : m_memory(memory), m_entityAddress(entityAddress) {
}

bool Entity::IsValid() const {
    return m_entityAddress != 0;
}

int Entity::GetHealth() const {
    return m_memory->Read<int>(m_entityAddress + offsets::netvars::m_iHealth);
}

int Entity::GetTeam() const {
    return m_memory->Read<int>(m_entityAddress + offsets::netvars::m_iTeamNum);
}

bool Entity::IsDormant() const {
    return m_memory->Read<bool>(m_entityAddress + offsets::netvars::m_bDormant);
}

Vector3 Entity::GetOrigin() const {
    return m_memory->Read<Vector3>(m_entityAddress + offsets::netvars::m_vecOrigin);
}

Vector3 Entity::GetEyePosition() const {
    Vector3 origin = GetOrigin();
    Vector3 viewOffset = m_memory->Read<Vector3>(m_entityAddress + offsets::netvars::m_vecViewOffset);
    return origin + viewOffset;
}

Vector3 Entity::GetViewOffset() const
{
    return Vector3();
}

Vector3 Entity::GetBonePosition(int boneID) const {
    DWORD boneMatrix = m_memory->Read<DWORD>(m_entityAddress + offsets::netvars::m_dwBoneMatrix);

    Vector3 bonePos;
    bonePos.x = m_memory->Read<float>(boneMatrix + 0x30 * boneID + 0x0C);
    bonePos.y = m_memory->Read<float>(boneMatrix + 0x30 * boneID + 0x1C);
    bonePos.z = m_memory->Read<float>(boneMatrix + 0x30 * boneID + 0x2C);

    return bonePos;
}

bool Entity::IsAlive() const {
    return GetHealth() > 0;
}

bool Entity::IsEnemy(const Entity& localPlayer) const {
    return GetTeam() != localPlayer.GetTeam();
}

Vector3 Entity::GetAimPunchAngle() const {
    return m_memory->Read<Vector3>(m_entityAddress + offsets::netvars::m_aimPunchAngle);
}

bool Entity::IsDefusing() const {
    return m_memory->Read<bool>(m_entityAddress + offsets::netvars::m_bIsDefusing);
}

float Entity::GetFlashDuration() const {
    return m_memory->Read<float>(m_entityAddress + offsets::netvars::m_flFlashDuration);
}

int Entity::GetShotsFired() const {
    return m_memory->Read<int>(m_entityAddress + offsets::netvars::m_iShotsFired);
}

int Entity::GetCrosshairID() const {
    return m_memory->Read<int>(m_entityAddress + offsets::netvars::m_iCrosshairId);
}

bool Entity::IsScoped() const {
    return m_memory->Read<bool>(m_entityAddress + offsets::netvars::m_bIsScoped);
}

int Entity::GetFlags() const {
    return m_memory->Read<int>(m_entityAddress + offsets::netvars::m_fFlags);
}

bool Entity::IsOnGround() const {
    return (GetFlags() & (1 << 0)) != 0;
}

uintptr_t Entity::GetActiveWeapon() const {
    // Lire le handle de l'arme active
    uintptr_t weaponHandle = m_memory->Read<uintptr_t>(m_entityAddress + offsets::netvars::m_hActiveWeapon);
    if (!weaponHandle) return 0;

    // Convertir le handle en pointeur d'entité
    return m_memory->Read<uintptr_t>(m_memory->GetClientBase() + offsets::signatures::dwEntityList + ((weaponHandle & 0x7FFF) * 0x10));
}

int Entity::GetWeaponID() const {
    DWORD weaponEntity = GetActiveWeapon();
    if (!weaponEntity) return 0;

    return m_memory->Read<int>(weaponEntity + offsets::netvars::m_iItemDefinitionIndex);
}
