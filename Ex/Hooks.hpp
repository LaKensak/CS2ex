#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <cstdint>

// Classe pour gérer les hooks
class Hooks {
public:
    // Initialiser les hooks
    static bool Initialize();

    // Supprimer les hooks
    static void Shutdown();

    // Types de fonctions pour les hooks
    typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    typedef LRESULT(__stdcall* WNDPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Fonctions originales
    static inline Present_t oPresent = nullptr;
    static inline WNDPROC oWndProc = nullptr;

    // Fonctions de hook
    static HRESULT __stdcall HkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static LRESULT __stdcall HkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Fenêtre du jeu
    static inline HWND GameWindow = nullptr;
};