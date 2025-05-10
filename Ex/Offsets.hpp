#pragma once
#include <cstdint>

// Offsets mis à jour pour CS2 (dernière version)
namespace offsets
{
    namespace signatures {
        // Client.dll signatures
        constexpr auto dwLocalPlayerController = 0x1A459E0;  // Vérifiez dans client_dll.hpp
        constexpr auto dwLocalPlayerPawn = 0x184B0D0;        // Vérifiez dans client_dll.hpp
        constexpr auto dwEntityList = 0x19F6F00;             // Vérifiez dans client_dll.hpp
        constexpr auto dwViewMatrix = 0x1A5FED0;             // Vérifiez dans client_dll.hpp
        constexpr auto dwGlowManager = 0x1A5AF20;            // Vérifiez dans client_dll.hpp
        constexpr auto dwGameRules = 0x1A5B7D8;              // Vérifiez dans client_dll.hpp
        constexpr auto dwCSGOInput = 0x1A69EF0;              // Vérifiez dans client_dll.hpp
        constexpr auto dwViewAngles = 0x1A6A2C0;             // Vérifiez dans client_dll.hpp
        constexpr auto dwPlantedC4 = 0x1A660B8;              // Vérifiez dans client_dll.hpp
        constexpr auto dwGlobalVars = 0x183F148;             // Vérifiez dans client_dll.hpp
        constexpr auto dwPrediction = 0x184AF50;             // Vérifiez dans client_dll.hpp
        constexpr auto dwGameEntitySystem = 0x1B1A718;       // Vérifiez dans client_dll.hpp
        constexpr auto dwGameEntitySystem_highestEntityIndex = 0x20F0;

        // Engine2.dll signatures
        constexpr auto dwNetworkGameClient = 0x540CE0;       // Vérifiez dans engine2_dll.hpp
        constexpr auto dwNetworkGameClient_clientTickCount = 0x368;
        constexpr auto dwNetworkGameClient_deltaTick = 0x27C;
        constexpr auto dwNetworkGameClient_localPlayer = 0xF0;
        constexpr auto dwNetworkGameClient_maxClients = 0x238;
        constexpr auto dwNetworkGameClient_serverTickCount = 0x36C;
        constexpr auto dwNetworkGameClient_signOnState = 0x228;
        constexpr auto dwWindowHeight = 0x62456C;            // Vérifiez dans engine2_dll.hpp
        constexpr auto dwWindowWidth = 0x624568;             // Vérifiez dans engine2_dll.hpp

        // InputSystem.dll offsets
        constexpr auto dwInputSystem = 0x387E0;

        // SkinChanger Signatures
        constexpr auto dwItemSchema = 0x173C7D0;             // Vérifiez dans client_dll.hpp
        constexpr auto m_bInitialized = 0x3C8;
        constexpr auto dwForceWeaponUpdate = 0x16C6B80;      // Vérifiez dans client_dll.hpp
    }

    namespace netvars {
        // Netvar offsets - vérifiez dans client_dll.hpp ou server_dll.hpp
        constexpr auto m_iHealth = 0x344;
        constexpr auto m_iTeamNum = 0x3E3;
        constexpr auto m_vecOrigin = 0x88;
        constexpr auto m_vecViewOffset = 0xCB0;
        constexpr auto m_aimPunchAngle = 0x1584;
        constexpr auto m_bSpotted = 0x8;
        constexpr auto m_iShotsFired = 0x23FC;
        constexpr auto m_bIsDefusing = 0x23EA;
        constexpr auto m_flFlashDuration = 0x140C;
        constexpr auto m_bDormant = 0xEF;
        constexpr auto m_fFlags = 0x3EC;
        constexpr auto m_iCrosshairId = 0x11838;
        constexpr auto m_dwBoneMatrix = 0x1124;
        constexpr auto m_bSpottedByMask = 0xC;
        constexpr auto m_hActiveWeapon = 0x0;
        constexpr auto m_iItemDefinitionIndex = 0x1BA;

        // Offsets supplémentaires pour CS2
        constexpr auto m_flC4Blow = 0xFC0;
        constexpr auto m_bBombTicking = 0xF90;
        constexpr auto m_bBombDefused = 0xFE4;
        constexpr auto m_hBombDefuser = 0xFE8;
        constexpr auto m_flDefuseCountDown = 0xFE0;
        constexpr auto m_flTimerLength = 0xFC8;
        constexpr auto m_bHasHelmet = 0x41;
        constexpr auto m_bHasHeavyArmor = 0x42;
        constexpr auto m_ArmorValue = 0x241C;
        constexpr auto m_bIsScoped = 0x23E8;
        constexpr auto m_bGunGameImmunity = 0x13BC;
        constexpr auto m_iClip1 = 0x1678;
        constexpr auto m_iPrimaryReserveAmmoCount = 0x1674;
        constexpr auto m_iObserverMode = 0x40;
        constexpr auto m_hObserverTarget = 0x44;

        // Offsets SkinChanger
        constexpr auto m_hMyWeapons = 0x40;
        constexpr auto m_AttributeManager = 0xFF0;
        constexpr auto m_Item = 0x50;
        constexpr auto m_iItemIDHigh = 0x1D0;
        constexpr auto m_iAccountID = 0x1D8;
        constexpr auto m_nFallbackPaintKit = 0x15F8;
        constexpr auto m_nFallbackSeed = 0x15FC;
        constexpr auto m_flFallbackWear = 0x1600;
        constexpr auto m_nFallbackStatTrak = 0x1604;
        constexpr auto m_iEntityQuality = 0x1BC;
    }
}