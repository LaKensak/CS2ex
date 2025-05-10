#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>

class Memory {
private:
    HANDLE m_hProcess;
    DWORD m_dwProcessId;
    DWORD m_dwClientBase;
    DWORD m_dwEngineBase;

public:
    Memory();
    ~Memory();

    bool Initialize(const wchar_t* processName, const wchar_t* moduleNameClient = L"client.dll", const wchar_t* moduleNameEngine = L"engine2.dll");

    DWORD GetProcessId(const wchar_t* processName);
    DWORD GetModuleBaseAddress(const wchar_t* moduleName);

    DWORD GetClientBase() const { return m_dwClientBase; }
    DWORD GetEngineBase() const { return m_dwEngineBase; }

    template<typename T>
    T Read(DWORD address) {
        T value{};
        ReadProcessMemory(m_hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
        return value;
    }

    template<typename T>
    bool Write(DWORD address, const T& value) {
        return WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(address), &value, sizeof(T), nullptr) != 0;
    }

    template<typename T>
    bool ReadArray(DWORD address, T* buffer, size_t count) {
        return ReadProcessMemory(m_hProcess, reinterpret_cast<LPCVOID>(address), buffer, sizeof(T) * count, nullptr) != 0;
    }
};