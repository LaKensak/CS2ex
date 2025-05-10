#include "SkinChanger.hpp"
#include "Offsets.hpp"
#include <thread>
#include <chrono>

SkinChanger::SkinChanger(Memory& mem) : memory(mem), initialized(false) {}

bool SkinChanger::Initialize() {
    initialized = true;
    return true;
}

void SkinChanger::Run() {
    if (!initialized || !Config::skinChanger.enabled) return;

    uintptr_t localPlayerPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwLocalPlayerController);
    if (!localPlayerPtr) return;

    Entity localPlayer(&memory, localPlayerPtr);
    if (!localPlayer.IsValid() || !localPlayer.IsAlive()) return;

    // Obtenir l'adresse de l'entité du joueur local (pas le controller)
    uintptr_t localPlayerPawn = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwLocalPlayerPawn);
    if (!localPlayerPawn) return;

    // Parcourir toutes les armes du joueur
    for (int i = 0; i < 8; i++) {
        // Lire le handle de l'arme
        uintptr_t weaponHandle = memory.Read<uintptr_t>(localPlayerPawn + offsets::netvars::m_hMyWeapons + (i * 0x8));
        if (!weaponHandle) continue;

        // Convertir le handle en pointeur d'entité
        uintptr_t weaponEntity = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwEntityList + ((weaponHandle & 0x7FFF) * 0x10));
        if (!weaponEntity) continue;

        // Obtenir l'index de définition de l'arme
        int weaponIndex = memory.Read<int>(weaponEntity + offsets::netvars::m_AttributeManager + offsets::netvars::m_Item + offsets::netvars::m_iItemDefinitionIndex);

        // Déterminer le skin à appliquer en fonction de l'arme
        int skinID = 0;
        switch (weaponIndex) {
        case 7:  // AK-47
            skinID = Config::skinChanger.ak47Skin;
            break;
        case 9:  // AWP
            skinID = Config::skinChanger.awpSkin;
            break;
        case 61: // USP-S
            skinID = Config::skinChanger.uspSkin;
            break;
        case 4:  // Glock-18
            skinID = Config::skinChanger.glockSkin;
            break;
        case 1:  // Desert Eagle
            skinID = Config::skinChanger.deagleSkin;
            break;
        case 40: // SSG 08
            skinID = Config::skinChanger.ssgSkin;
            break;
        case 16: // M4A4
            skinID = Config::skinChanger.m4a4Skin;
            break;
        case 60: // M4A1-S
            skinID = Config::skinChanger.m4a1sSkin;
            break;
        default:
            continue;
        }

        // Appliquer le skin
        if (skinID > 0) {
            // Forcer la mise à jour de l'arme
            memory.Write<int>(weaponEntity + offsets::netvars::m_iItemIDHigh, -1);

            // Appliquer les propriétés du skin
            memory.Write<int>(weaponEntity + offsets::netvars::m_AttributeManager + offsets::netvars::m_Item + offsets::netvars::m_iAccountID, 1);
            memory.Write<int>(weaponEntity + offsets::netvars::m_AttributeManager + offsets::netvars::m_Item + offsets::netvars::m_iEntityQuality, 0);
            memory.Write<int>(weaponEntity + offsets::netvars::m_nFallbackPaintKit, skinID);
            memory.Write<float>(weaponEntity + offsets::netvars::m_flFallbackWear, 0.01f); // Usure minimale (presque neuf)
            memory.Write<int>(weaponEntity + offsets::netvars::m_nFallbackSeed, 0);
            memory.Write<int>(weaponEntity + offsets::netvars::m_nFallbackStatTrak, -1); // Pas de StatTrak
        }
    }

    // Forcer la mise à jour des skins
    static auto lastUpdate = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count() > 500) {
        // Forcer la mise à jour en changeant d'arme
        uintptr_t activeWeapon = localPlayer.GetActiveWeapon();
        if (activeWeapon) {
            // Simuler un changement d'arme pour forcer la mise à jour
            memory.Write<int>(memory.GetClientBase() + offsets::signatures::dwForceWeaponUpdate, -1);
        }
        lastUpdate = now;
    }
}