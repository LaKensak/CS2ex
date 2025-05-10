#include "Memory.hpp"
#include <iostream>

// Fonction pour obtenir l'adresse de base d'un module
uintptr_t Memory::GetModuleBaseAddress(const char* moduleName) {
    return (uintptr_t)GetModuleHandleA(moduleName);
}

// Fonction pour initialiser la mémoire
bool Memory::Initialize() {
    try {
        std::cout << "Finding module base addresses..." << std::endl;

        // Obtenir les adresses de base des modules
        ClientBase = GetModuleBaseAddress("client.dll");
        if (!ClientBase) {
            std::cout << "Failed to find client.dll module!" << std::endl;
            ClientBase = GetModuleBaseAddress("cs2_client.dll"); // Essayer le nom alternatif
            if (!ClientBase) {
                std::cout << "Failed to find cs2_client.dll module!" << std::endl;
                return false;
            }
        }
        std::cout << "Client module found at: 0x" << std::hex << ClientBase << std::dec << std::endl;

        EngineBase = GetModuleBaseAddress("engine.dll");
        if (!EngineBase) {
            std::cout << "Failed to find engine.dll module!" << std::endl;
            EngineBase = GetModuleBaseAddress("cs2_engine.dll"); // Essayer le nom alternatif
            if (!EngineBase) {
                std::cout << "Failed to find cs2_engine.dll module!" << std::endl;
                return false;
            }
        }
        std::cout << "Engine module found at: 0x" << std::hex << EngineBase << std::dec << std::endl;

        // Rechercher les signatures et résoudre les pointeurs
        std::cout << "Finding signatures..." << std::endl;

        // Trouver LocalPlayer
        uintptr_t localPlayerSig = FindPattern("client.dll", "48 8B 05 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 06 48 8B 49 08");
        if (localPlayerSig) {
            // Résoudre le déplacement relatif (RIP-relative)
            int32_t offset = *(int32_t*)(localPlayerSig + 3);
            LocalPlayer = (void*)(localPlayerSig + 7 + offset);
            std::cout << "LocalPlayer found at: 0x" << std::hex << (uintptr_t)LocalPlayer << std::dec << std::endl;
        }
        else {
            std::cout << "Failed to find LocalPlayer signature!" << std::endl;
            return false;
        }

        // Trouver EntityList
        uintptr_t entityListSig = FindPattern("client.dll", "48 8B 0D ? ? ? ? 48 89 7C 24 ? 8B FA C1 EB 06");
        if (entityListSig) {
            // Résoudre le déplacement relatif (RIP-relative)
            int32_t offset = *(int32_t*)(entityListSig + 3);
            EntityList = (void*)(entityListSig + 7 + offset);
            std::cout << "EntityList found at: 0x" << std::hex << (uintptr_t)EntityList << std::dec << std::endl;
        }
        else {
            std::cout << "Failed to find EntityList signature!" << std::endl;
            return false;
        }

        // Trouver ViewMatrix
        uintptr_t viewMatrixSig = FindPattern("client.dll", "48 8D 0D ? ? ? ? 48 C1 E0 06");
        if (viewMatrixSig) {
            // Résoudre le déplacement relatif (RIP-relative)
            int32_t offset = *(int32_t*)(viewMatrixSig + 3);
            ViewMatrix = (void*)(viewMatrixSig + 7 + offset);
            std::cout << "ViewMatrix found at: 0x" << std::hex << (uintptr_t)ViewMatrix << std::dec << std::endl;
        }
        else {
            std::cout << "Failed to find ViewMatrix signature!" << std::endl;
            return false;
        }

        std::cout << "Memory initialization complete!" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Exception during memory initialization: " << e.what() << std::endl;
        return false;
    }
}

// Fonction pour trouver un motif dans la mémoire
uintptr_t Memory::FindPattern(const char* module, const char* pattern) {
    MODULEINFO modInfo;
    HMODULE hModule = GetModuleHandleA(module);

    if (!hModule) {
        // Essayer le nom alternatif pour CS2
        if (strcmp(module, "client.dll") == 0) {
            hModule = GetModuleHandleA("cs2_client.dll");
        }
        else if (strcmp(module, "engine.dll") == 0) {
            hModule = GetModuleHandleA("cs2_engine.dll");
        }

        if (!hModule) {
            return 0;
        }
    }

    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

    uintptr_t start = (uintptr_t)modInfo.lpBaseOfDll;
    uintptr_t end = start + modInfo.SizeOfImage;

    // Fonction pour convertir un caractère hexadécimal en valeur
    auto HexCharToInt = [](char c) -> int {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        return -1;
        };

    // Convertir le pattern en bytes
    std::vector<int> bytes;
    for (const char* ptr = pattern; *ptr; ++ptr) {
        if (*ptr == ' ')
            continue;

        if (*ptr == '?') {
            bytes.push_back(-1);
            if (*(ptr + 1) == '?')
                ++ptr;
        }
        else {
            if (*(ptr + 1) && *(ptr + 1) != ' ') {
                int high = HexCharToInt(*ptr);
                int low = HexCharToInt(*(ptr + 1));
                if (high != -1 && low != -1) {
                    bytes.push_back((high << 4) | low);
                }
                ++ptr;
            }
        }
    }

    // Rechercher le pattern
    for (uintptr_t i = start; i < end - bytes.size(); ++i) {
        bool found = true;
        for (size_t j = 0; j < bytes.size(); ++j) {
            if (bytes[j] != -1 && *(uint8_t*)(i + j) != bytes[j]) {
                found = false;
                break;
            }
        }

        if (found) {
            return i;
        }
    }

    return 0;
}

// Fonction pour trouver une interface
void* Memory::FindInterface(const char* module, const char* interfaceName) {
    // Obtenir la fonction CreateInterface
    typedef void* (*CreateInterfaceFn)(const char*, int*);
    HMODULE hModule = GetModuleHandleA(module);

    if (!hModule) {
        // Essayer le nom alternatif pour CS2
        if (strcmp(module, "client.dll") == 0) {
            hModule = GetModuleHandleA("cs2_client.dll");
        }
        else if (strcmp(module, "engine.dll") == 0) {
            hModule = GetModuleHandleA("cs2_engine.dll");
        }

        if (!hModule) {
            return nullptr;
        }
    }

    CreateInterfaceFn createInterface = (CreateInterfaceFn)GetProcAddress(hModule, "CreateInterface");
    if (!createInterface) {
        return nullptr;
    }

    return createInterface(interfaceName, nullptr);
}