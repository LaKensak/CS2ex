#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <Psapi.h>

// Classe pour g�rer l'acc�s � la m�moire du jeu
class Memory {
public:
    // Initialiser les pointeurs et trouver les adresses n�cessaires
    static bool Initialize();

    // Obtenir l'adresse de base d'un module
    static uintptr_t GetModuleBaseAddress(const char* moduleName);

    // Lire la m�moire � une adresse donn�e
    template <typename T>
    static T Read(uintptr_t address) {
        if (address <= 0x10000 || address >= 0x7FFFFFFFFFFF) {
            return T();
        }
        try {
            return *(T*)address;
        }
        catch (...) {
            return T();
        }
    }

    // �crire dans la m�moire � une adresse donn�e
    template <typename T>
    static void Write(uintptr_t address, T value) {
        if (address <= 0x10000 || address >= 0x7FFFFFFFFFFF) {
            return;
        }
        try {
            *(T*)address = value;
        }
        catch (...) {
            // G�rer silencieusement les erreurs
        }
    }

    // Trouver un motif dans un module
    static uintptr_t FindPattern(const char* module, const char* pattern);

    // Trouver une interface du jeu
    static void* FindInterface(const char* module, const char* interfaceName);

    // Pointeurs vers les structures importantes du jeu
    static inline uintptr_t ClientBase = 0;
    static inline uintptr_t EngineBase = 0;
    static inline void* LocalPlayer = nullptr;
    static inline void* EntityList = nullptr;
    static inline void* ViewMatrix = nullptr;
};