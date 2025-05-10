#include <Windows.h>
#include <iostream>
#include <thread>
#include "Memory.h"
#include "Features.h"

// Variable globale pour savoir si le cheat est actif
bool g_Running = true;

// Fonction pour initialiser la console de débogage
void SetupConsole() {
    AllocConsole();
    FILE* pFile = nullptr;
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    SetConsoleTitleA("CS2 Cheat Debug Console");
    std::cout << "=== CS2 Cheat Loaded ===" << std::endl;
    std::cout << "Press END key to unload the cheat." << std::endl;
}

// Thread principal pour le cheat
DWORD WINAPI MainThread(LPVOID lpParam) {
    try {
        // Initialiser la console de débogage
        SetupConsole();
        std::cout << "Initializing cheat components..." << std::endl;

        // Initialiser la mémoire
        std::cout << "Initializing memory..." << std::endl;
        if (!Memory::Initialize()) {
            std::cout << "Failed to initialize memory. Exiting..." << std::endl;
            g_Running = false;
            return 0;
        }
        std::cout << "Memory initialized successfully." << std::endl;

        // Initialiser les hooks et les fonctionnalités
        std::cout << "Initializing features..." << std::endl;
        if (!Features::Initialize()) {
            std::cout << "Failed to initialize features. Exiting..." << std::endl;
            g_Running = false;
            return 0;
        }
        std::cout << "Features initialized successfully." << std::endl;

        std::cout << "Cheat initialization complete!" << std::endl;
        std::cout << "Entering main loop..." << std::endl;

        // Boucle principale du cheat
        while (g_Running) {
            // Vérifier si l'utilisateur a appuyé sur END pour quitter
            if (GetAsyncKeyState(VK_END) & 1) {
                std::cout << "END key pressed. Unloading cheat..." << std::endl;
                g_Running = false;
                break;
            }

            // Mettre à jour les fonctionnalités du cheat
            Features::Update();

            // Empêcher l'utilisation à 100% du CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Nettoyer avant de quitter
        std::cout << "Cleaning up resources..." << std::endl;
        Features::Shutdown();
        std::cout << "Cleanup complete. Unloading DLL..." << std::endl;

        // Fermer la console
        FreeConsole();
    }
    catch (const std::exception& e) {
        // Capturer et afficher les exceptions
        MessageBoxA(NULL, e.what(), "CS2Cheat Error", MB_ICONERROR);
    }

    // Libérer le DLL
    FreeLibraryAndExitThread((HMODULE)lpParam, 0);
    return 0;
}

// Point d'entrée du DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Désactiver les appels de thread pour améliorer les performances
        DisableThreadLibraryCalls(hModule);

        // Créer un thread séparé pour le cheat
        HANDLE hThread = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        if (hThread) {
            // Fermer le handle du thread car nous n'en avons plus besoin
            CloseHandle(hThread);
        }
        else {
            // Afficher une erreur si la création du thread a échoué
            char errorMsg[128];
            sprintf_s(errorMsg, "Failed to create cheat thread. Error code: %d", GetLastError());
            MessageBoxA(NULL, errorMsg, "CS2Cheat Error", MB_ICONERROR);
            return FALSE;
        }
        break;

    case DLL_PROCESS_DETACH:
        // Assurer que le cheat s'arrête proprement
        g_Running = false;
        // Donner du temps au thread pour se terminer
        Sleep(100);
        break;
    }

    return TRUE;
}