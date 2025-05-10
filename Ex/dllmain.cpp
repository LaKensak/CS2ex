#include <Windows.h>
#include <d3d9.h>
#include "MinHook.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "CheatManager.hpp"
#include "Logger.hpp"
#include "Config.hpp"

// Déclarations de types
typedef HRESULT(APIENTRY* EndScene)(IDirect3DDevice9* pDevice);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

// Variables globales
HWND window = nullptr;
WNDPROC originalWndProc = nullptr;
EndScene originalEndScene = nullptr;
CheatManager cheatManager;
bool menuOpen = false;
bool initialized = false;

// Déclaration externe pour ImGui
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Hook WndProc
LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Gérer les entrées ImGui si le menu est ouvert
    if (menuOpen && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    // Ouvrir/fermer le menu avec INSERT
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
        menuOpen = !menuOpen;
        Logger::Log(menuOpen ? "Menu opened" : "Menu closed");
    }

    return CallWindowProc(originalWndProc, hWnd, uMsg, wParam, lParam);
}

// Hook EndScene
HRESULT APIENTRY hkEndScene(IDirect3DDevice9* pDevice) {
    if (!initialized) {
        Logger::Log("Initializing ImGui...");

        // Obtenir le handle de la fenêtre
        D3DDEVICE_CREATION_PARAMETERS params;
        pDevice->GetCreationParameters(&params);
        window = params.hFocusWindow;

        // Initialiser ImGui
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

        // Initialiser les implémentations ImGui
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX9_Init(pDevice);

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

        // Initialiser le cheat manager
        cheatManager.SetDevice(pDevice);
        if (cheatManager.Initialize()) {
            cheatManager.Start();
            Logger::Log("CheatManager started successfully");
        }
        else {
            Logger::Error("Failed to initialize CheatManager");
        }

        initialized = true;
        Logger::Log("Initialization complete");
    }

    // Exécuter les fonctionnalités du cheat
    cheatManager.RunFeatures();

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

                ImGui::EndTabItem();
            }

            // Onglet ESP
            if (ImGui::BeginTabItem("ESP")) {
                ImGui::Checkbox("Enable ESP", &Config::esp.enabled);
                ImGui::Checkbox("Boxes", &Config::esp.boxes);
                ImGui::Checkbox("Health Bar", &Config::esp.health);
                ImGui::Checkbox("Name", &Config::esp.name);
                ImGui::Checkbox("Snaplines", &Config::esp.snaplines);
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

// Fonction pour trouver l'adresse de EndScene
void* FindEndScene() {
    Logger::Log("Finding EndScene address...");

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"DX", NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(L"DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D) {
        Logger::Error("Failed to create D3D9 interface");
        return nullptr;
    }

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hwnd;

    IDirect3DDevice9* pDevice;
    HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);

    if (FAILED(result) || !pDevice) {
        Logger::Error("Failed to create D3D9 device: " + std::to_string(result));
        if (pD3D) pD3D->Release();
        DestroyWindow(hwnd);
        UnregisterClass(L"DX", wc.hInstance);
        return nullptr;
    }

    void* endSceneAddr = (void*)((*(void***)pDevice)[42]); // 42 est l'index de EndScene dans la vtable
    Logger::Log("EndScene address found: " + std::to_string((uintptr_t)endSceneAddr));

    pDevice->Release();
    pD3D->Release();
    DestroyWindow(hwnd);
    UnregisterClass(L"DX", wc.hInstance);

    return endSceneAddr;
}

// Fonction d'initialisation
DWORD WINAPI MainThread(LPVOID lpParam) {
    // Initialiser le logger
    Logger::Init();
    Logger::Log("DLL injected successfully");

    // Initialiser MinHook
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK) {
        Logger::Error("Failed to initialize MinHook: " + std::to_string(status));
        return 0;
    }
    Logger::Log("MinHook initialized successfully");

    // Trouver l'adresse de EndScene
    void* endSceneAddr = FindEndScene();
    if (!endSceneAddr) {
        Logger::Error("Failed to find EndScene address");
        return 0;
    }

    // Créer le hook pour EndScene
    status = MH_CreateHook(endSceneAddr, hkEndScene, (void**)&originalEndScene);
    if (status != MH_OK) {
        Logger::Error("Failed to create EndScene hook: " + std::to_string(status));
        return 0;
    }
    Logger::Log("EndScene hook created successfully");

    // Activer le hook
    status = MH_EnableHook(endSceneAddr);
    if (status != MH_OK) {
        Logger::Error("Failed to enable EndScene hook: " + std::to_string(status));
        return 0;
    }
    Logger::Log("EndScene hook enabled successfully");

    return 0;
}

// Point d'entrée de la DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        if (initialized) {
            cheatManager.Stop();

            MH_DisableHook(MH_ALL_HOOKS);
            MH_Uninitialize();

            if (originalWndProc && window) {
                SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)originalWndProc);
            }

            ImGui_ImplDX9_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            Logger::Shutdown();
        }
        break;
    }
    return TRUE;
}