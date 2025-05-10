// Correction pour dllmain.cpp
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "MinHook.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "CheatManager.hpp"
#include "Logger.hpp"

// Déclaration des types de fonctions pour les hooks DirectX
typedef HRESULT(APIENTRY* EndScene)(IDirect3DDevice9* pDevice);
typedef HRESULT(APIENTRY* Reset)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

// Variables globales
HWND window = nullptr;
WNDPROC originalWndProc = nullptr;
EndScene originalEndScene = nullptr;
Reset originalReset = nullptr;
CheatManager cheatManager;
bool menuOpen = false;
bool initialized = false;

// Déclaration externe pour ImGui_ImplWin32_WndProcHandler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Fonction de hook pour WndProc
LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Gérer les entrées ImGui
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    // Ouvrir/fermer le menu avec INSERT
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
        menuOpen = !menuOpen;
        Logger::Log(menuOpen ? "Menu opened" : "Menu closed");
    }

    return CallWindowProc(originalWndProc, hWnd, uMsg, wParam, lParam);
}

// Fonction de hook pour EndScene
HRESULT APIENTRY hkEndScene(IDirect3DDevice9* pDevice) {
    if (!initialized) {
        Logger::Log("EndScene hook called for the first time");

        // Initialiser ImGui
        D3DDEVICE_CREATION_PARAMETERS params;
        pDevice->GetCreationParameters(&params);
        window = params.hFocusWindow;

        Logger::Log("Window handle obtained: " + std::to_string((uintptr_t)window));

        // Configurer ImGui
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

        if (!ImGui_ImplWin32_Init(window)) {
            Logger::Error("Failed to initialize ImGui Win32 implementation");
            return originalEndScene(pDevice);
        }

        if (!ImGui_ImplDX9_Init(pDevice)) {
            Logger::Error("Failed to initialize ImGui DX9 implementation");
            return originalEndScene(pDevice);
        }

        Logger::Log("ImGui initialized successfully");

        // Configurer le style ImGui
        ImGui::StyleColorsDark();

        // Hook WndProc
        originalWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
        if (!originalWndProc) {
            Logger::Error("Failed to hook WndProc: " + std::to_string(GetLastError()));
        }
        else {
            Logger::Log("WndProc hooked successfully");
        }

        // Initialiser le cheat manager avec le device
        cheatManager.SetDevice(pDevice);
        if (!cheatManager.Initialize()) {
            Logger::Error("Failed to initialize CheatManager");
        }
        else {
            cheatManager.Start();
            Logger::Log("CheatManager started successfully");
        }

        initialized = true;
    }

    // Rendre l'ESP
    if (Config::esp.enabled && pDevice) {
        cheatManager.RunFeatures();
    }

    // Rendre le menu ImGui si ouvert
    if (menuOpen) {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Fenêtre principale
        ImGui::Begin("CS2 Cheat", &menuOpen);

        // Onglets
        if (ImGui::BeginTabBar("CheatTabs")) {
            // Onglet Aimbot
            if (ImGui::BeginTabItem("Aimbot")) {
                ImGui::Checkbox("Enable Aimbot", &Config::aimbot.enabled);
                ImGui::SliderFloat("FOV", &Config::aimbot.fov, 0.1f, 30.0f, "%.1f");
                ImGui::SliderFloat("Smooth", &Config::aimbot.smooth, 1.0f, 20.0f, "%.1f");
                ImGui::Checkbox("Recoil Control", &Config::aimbot.recoilControl);
                ImGui::Checkbox("Team Check", &Config::aimbot.teamCheck);

                const char* bones[] = { "Head", "Neck", "Chest", "Stomach" };
                ImGui::Combo("Bone", &Config::aimbot.bone, bones, IM_ARRAYSIZE(bones));

                ImGui::Text("Aimbot Key: %d", Config::aimbot.key);

                ImGui::EndTabItem();
            }

            // Onglet ESP
            if (ImGui::BeginTabItem("ESP")) {
                ImGui::Checkbox("Enable ESP", &Config::esp.enabled);
                ImGui::Checkbox("Boxes", &Config::esp.boxes);
                ImGui::Checkbox("Health Bar", &Config::esp.health);
                ImGui::Checkbox("Health Text", &Config::esp.healthText);
                ImGui::Checkbox("Name", &Config::esp.name);
                ImGui::Checkbox("Snaplines", &Config::esp.snaplines);
                ImGui::Checkbox("Head Dot", &Config::esp.headDot);
                ImGui::Checkbox("Team Check", &Config::esp.teamCheck);
                ImGui::EndTabItem();
            }

            // Onglet Skins
            if (ImGui::BeginTabItem("Skins")) {
                ImGui::Checkbox("Enable Skin Changer", &Config::skinChanger.enabled);

                ImGui::InputInt("AK-47 Skin", &Config::skinChanger.ak47Skin);
                ImGui::InputInt("AWP Skin", &Config::skinChanger.awpSkin);
                ImGui::InputInt("USP-S Skin", &Config::skinChanger.uspSkin);
                ImGui::InputInt("Glock-18 Skin", &Config::skinChanger.glockSkin);
                ImGui::InputInt("Desert Eagle Skin", &Config::skinChanger.deagleSkin);
                ImGui::InputInt("SSG 08 Skin", &Config::skinChanger.ssgSkin);
                ImGui::InputInt("M4A4 Skin", &Config::skinChanger.m4a4Skin);
                ImGui::InputInt("M4A1-S Skin", &Config::skinChanger.m4a1sSkin);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return originalEndScene(pDevice);
}

// Fonction de hook pour Reset
HRESULT APIENTRY hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT result = originalReset(pDevice, pPresentationParameters);
    ImGui_ImplDX9_CreateDeviceObjects();
    return result;
}

// Fonction pour obtenir l'adresse de la vtable
DWORD_PTR GetVTableFunction(const char* moduleName, DWORD vTableIndex) {
    HMODULE module = GetModuleHandleA(moduleName);
    if (!module) {
        Logger::Error("Failed to get module handle for " + std::string(moduleName));
        return 0;
    }

    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D) {
        Logger::Error("Failed to create D3D9 interface");
        return 0;
    }

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = GetForegroundWindow();

    IDirect3DDevice9* pDevice = nullptr;
    HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);

    if (FAILED(result) || !pDevice) {
        Logger::Error("Failed to create D3D9 device: " + std::to_string(result));
        if (pD3D) pD3D->Release();
        return 0;
    }

    void** vTable = *reinterpret_cast<void***>(pDevice);
    DWORD_PTR address = (DWORD_PTR)vTable[vTableIndex];

    pDevice->Release();
    pD3D->Release();

    return address;
}

// Fonction pour installer les hooks DirectX
bool InstallHooks() {
    Logger::Log("Installing hooks...");

    // Obtenir l'adresse de la vtable pour EndScene et Reset
    DWORD_PTR endSceneAddress = GetVTableFunction("d3d9.dll", 42);
    DWORD_PTR resetAddress = GetVTableFunction("d3d9.dll", 16);

    if (!endSceneAddress || !resetAddress) {
        Logger::Error("Failed to get DirectX function addresses");
        return false;
    }

    Logger::Log("EndScene address: " + std::to_string(endSceneAddress));
    Logger::Log("Reset address: " + std::to_string(resetAddress));

    // Initialiser MinHook
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK) {
        Logger::Error("Failed to initialize MinHook: " + std::to_string(status));
        return false;
    }

    Logger::Log("MinHook initialized");

    // Créer les hooks
    status = MH_CreateHook((LPVOID)endSceneAddress, hkEndScene, (LPVOID*)&originalEndScene);
    if (status != MH_OK) {
        Logger::Error("Failed to create EndScene hook: " + std::to_string(status));
        return false;
    }

    Logger::Log("EndScene hook created");

    status = MH_CreateHook((LPVOID)resetAddress, hkReset, (LPVOID*)&originalReset);
    if (status != MH_OK) {
        Logger::Error("Failed to create Reset hook: " + std::to_string(status));
        return false;
    }

    Logger::Log("Reset hook created");

    // Activer les hooks
    status = MH_EnableHook(MH_ALL_HOOKS);
    if (status != MH_OK) {
        Logger::Error("Failed to enable hooks: " + std::to_string(status));
        return false;
    }

    Logger::Log("Hooks enabled successfully");
    return true;
}

// Fonction pour désinstaller les hooks
void UninstallHooks() {
    Logger::Log("Uninstalling hooks...");

    // Désactiver et supprimer les hooks
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    if (originalWndProc && window) {
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)originalWndProc);
    }

    // Nettoyer ImGui
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Logger::Log("Hooks uninstalled successfully");
}

// Point d'entrée de la DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InstallHooks, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        cheatManager.Stop();
        UninstallHooks();
        Logger::Shutdown();
        break;
    }
    return TRUE;
}