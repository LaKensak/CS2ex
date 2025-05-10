#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <filesystem>

// Fonction pour obtenir l'ID du processus à partir de son nom
DWORD GetProcessId(const char* processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshot, &processEntry)) {
            do {
                // Convertir wchar_t en char pour la comparaison
                char exeName[MAX_PATH];
                wcstombs_s(nullptr, exeName, processEntry.szExeFile, MAX_PATH);

                if (_stricmp(exeName, processName) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }

    return processId;
}

// Fonction améliorée pour injecter une DLL
BOOL Inject(DWORD ProcessId, const char* dllPath) {
    std::cout << "Attempting to inject DLL into process ID: " << ProcessId << std::endl;
    std::cout << "DLL path: " << dllPath << std::endl;

    // Ouvrir le processus
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
    if (!hProcess) {
        std::cout << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return FALSE;
    }
    std::cout << "Process opened successfully." << std::endl;

    // Allouer de la mémoire dans le processus cible
    SIZE_T dllPathSize = strlen(dllPath) + 1;
    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pDllPath) {
        std::cout << "Failed to allocate memory in target process. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return FALSE;
    }
    std::cout << "Memory allocated at: 0x" << std::hex << pDllPath << std::dec << std::endl;

    // Écrire le chemin du DLL dans la mémoire allouée
    if (!WriteProcessMemory(hProcess, pDllPath, dllPath, dllPathSize, NULL)) {
        std::cout << "Failed to write DLL path to process memory. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    std::cout << "DLL path written to process memory." << std::endl;

    // Obtenir l'adresse de LoadLibraryA
    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (!pLoadLibrary) {
        std::cout << "Failed to get address of LoadLibraryA. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    std::cout << "LoadLibraryA found at: 0x" << std::hex << pLoadLibrary << std::dec << std::endl;

    // Créer un thread dans le processus cible pour charger la DLL
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pDllPath, 0, NULL);
    if (!hThread) {
        std::cout << "Failed to create remote thread. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    std::cout << "Remote thread created successfully." << std::endl;

    // Attendre que le thread se termine
    std::cout << "Waiting for thread completion..." << std::endl;
    WaitForSingleObject(hThread, INFINITE);

    // Vérifier le résultat du thread
    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);
    std::cout << "Thread completed with exit code: 0x" << std::hex << exitCode << std::dec << std::endl;

    // Nettoyer
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    if (exitCode == 0) {
        std::cout << "DLL injection failed." << std::endl;
        return FALSE;
    }

    std::cout << "DLL injected successfully!" << std::endl;
    return TRUE;
}

// Fonction principale
int main() {
    SetConsoleTitleA("CS2 DLL Injector");
    std::cout << "=== CS2 DLL Injector ===" << std::endl;
    std::cout << "Searching for CS2 process..." << std::endl;

    // Obtenir l'ID du processus CS2
    DWORD processId = GetProcessId("cs2.exe");
    if (processId == 0) {
        std::cout << "Failed to find CS2 process. Make sure the game is running." << std::endl;
        std::cout << "Press any key to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "CS2 Process found! ID: " << processId << std::endl;

    // Obtenir le chemin complet du DLL
    char dllPath[MAX_PATH];
    GetFullPathNameA("CS2Cheat.dll", MAX_PATH, dllPath, NULL);

    // Vérifier si le DLL existe
    if (!std::filesystem::exists(dllPath)) {
        std::cout << "DLL file not found: " << dllPath << std::endl;
        std::cout << "Make sure the DLL is in the same directory as the injector." << std::endl;
        std::cout << "Press any key to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "DLL found: " << dllPath << std::endl;
    std::cout << "Preparing to inject..." << std::endl;

    // Injecter le DLL
    if (Inject(processId, dllPath)) {
        std::cout << "Injection completed successfully!" << std::endl;
    }
    else {
        std::cout << "Injection failed. Try running as administrator." << std::endl;
    }

    std::cout << "Press any key to exit...";
    std::cin.get();
    return 0;
}