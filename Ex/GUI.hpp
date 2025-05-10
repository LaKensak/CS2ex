#pragma once
#include <Windows.h>

// Classe pour gérer l'interface utilisateur
class GUI {
public:
    // Initialiser l'interface
    static bool Initialize();

    // Mettre à jour l'interface
    static void Update();

    // Dessiner l'interface
    static void Render();

    // Arrêter l'interface
    static void Shutdown();

    // État de l'interface
    static inline bool Visible = false;
};