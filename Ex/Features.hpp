#pragma once
#include <Windows.h>
#include <cstdint>

// Classe pour gérer les fonctionnalités du cheat
class Features {
public:
    // Initialiser les fonctionnalités
    static bool Initialize();

    // Mettre à jour les fonctionnalités
    static void Update();

    // Arrêter les fonctionnalités avant de quitter
    static void Shutdown();

    // Activer/désactiver les fonctionnalités
    static inline bool ESP_Enabled = true;
    static inline bool Aimbot_Enabled = true;
    static inline bool Triggerbot_Enabled = false;
    static inline bool NoRecoil_Enabled = false;

    // Clés de touche pour activer les fonctionnalités
    static inline int Aimbot_Key = VK_LBUTTON; // Bouton gauche de la souris
    static inline int Triggerbot_Key = VK_XBUTTON1; // Bouton latéral de la souris

private:
    // Variables pour les fonctionnalités
    static inline float Aimbot_FOV = 5.0f;
    static inline float Aimbot_Smooth = 5.0f;

    // Fonctions pour les fonctionnalités
    static void ESP();
    static void Aimbot();
    static void Triggerbot();
    static void NoRecoil();
};
