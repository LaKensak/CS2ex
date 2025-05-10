#include "Memory.hpp"
#include <TlHelp32.h>
#include <iostream>

Memory::Memory() : m_hProcess(NULL), m_dwProcessId(0), m_dwClientBase(0), m_dwEngineBase(0) {}

Memory::~Memory() {
    if (m_hProcess) {
        CloseHandle(m_hProcess);
    }
}

bool Memory::Initialize(const wchar_t* processName, const wchar_t* moduleNameClient, const wchar_t* moduleNameEngine) {
    m_dwProcessId = GetProcessId(processName);
    if (!m_dwProcessId) {
        std::wcerr << L"Failed to find process: " << processName << std::endl;
        return false;
    }

    m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwProcessId);
    if (!m_hProcess) {
        std::cerr << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return false;
    }

    m_dwClientBase = GetModuleBaseAddress(moduleNameClient);
    if (!m_dwClientBase) {
        std::wcerr << L"Failed to find " << moduleNameClient << std::endl;
        return false;
    }

    m_dwEngineBase = GetModuleBaseAddress(moduleNameEngine);
    if (!m_dwEngineBase) {
        std::wcerr << L"Failed to find " << moduleNameEngine << std::endl;
        return false;
    }

    return true;
}

DWORD Memory::GetProcessId(const wchar_t* processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshot, &processEntry)) {
            do {
                if (wcscmp(processEntry.szExeFile, processName) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }

    return processId;
}

DWORD Memory::GetModuleBaseAddress(const wchar_t* moduleName) {
    DWORD moduleBase = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_dwProcessId);

    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(snapshot, &moduleEntry)) {
            do {
                if (wcscmp(moduleEntry.szModule, moduleName) == 0) {
                    moduleBase = (DWORD)moduleEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(snapshot, &moduleEntry));
        }
        CloseHandle(snapshot);
    }

    return moduleBase;
}