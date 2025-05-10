#pragma once
#include <Windows.h>

// Classe pour g�rer l'interface utilisateur
class GUI {
public:
    // Initialiser l'interface
    static bool Initialize();

    // Mettre � jour l'interface
    static void Update();

    // Dessiner l'interface
    static void Render();

    // Arr�ter l'interface
    static void Shutdown();

    // �tat de l'interface
    static inline bool Visible = false;
};