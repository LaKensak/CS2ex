#include <Windows.h>
#include <iostream>
#include <thread>
#include "Memory.h"
#include "Features.h"

// Variable globale pour savoir si le cheat est actif
bool g_Running = true;

// Fonction pour initialiser la console de d�bogage
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
        // Initialiser la console de d�bogage
        SetupConsole();
        std::cout << "Initializing cheat components..." << std::endl;

        // Initialiser la m�moire
        std::cout << "Initializing memory..." << std::endl;
        if (!Memory::Initialize()) {
            std::cout << "Failed to initialize memory. Exiting..." << std::endl;
            g_Running = false;
            return 0;
        }
        std::cout << "Memory initialized successfully." << std::endl;

        // Initialiser les hooks et les fonctionnalit�s
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
            // V�rifier si l'utilisateur a appuy� sur END pour quitter
            if (GetAsyncKeyState(VK_END) & 1) {
                std::cout << "END key pressed. Unloading cheat..." << std::endl;
                g_Running = false;
                break;
            }

            // Mettre � jour les fonctionnalit�s du cheat
            Features::Update();

            // Emp�cher l'utilisation � 100% du CPU
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

    // Lib�rer le DLL
    FreeLibraryAndExitThread((HMODULE)lpParam, 0);
    return 0;
}

// Point d'entr�e du DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // D�sactiver les appels de thread pour am�liorer les performances
        DisableThreadLibraryCalls(hModule);

        // Cr�er un thread s�par� pour le cheat
        HANDLE hThread = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        if (hThread) {
            // Fermer le handle du thread car nous n'en avons plus besoin
            CloseHandle(hThread);
        }
        else {
            // Afficher une erreur si la cr�ation du thread a �chou�
            char errorMsg[128];
            sprintf_s(errorMsg, "Failed to create cheat thread. Error code: %d", GetLastError());
            MessageBoxA(NULL, errorMsg, "CS2Cheat Error", MB_ICONERROR);
            return FALSE;
        }
        break;

    case DLL_PROCESS_DETACH:
        // Assurer que le cheat s'arr�te proprement
        g_Running = false;
        // Donner du temps au thread pour se terminer
        Sleep(100);
        break;
    }

    return TRUE;
}