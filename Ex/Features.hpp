#pragma once
#include <Windows.h>
#include <cstdint>

// Classe pour g�rer les fonctionnalit�s du cheat
class Features {
public:
    // Initialiser les fonctionnalit�s
    static bool Initialize();

    // Mettre � jour les fonctionnalit�s
    static void Update();

    // Arr�ter les fonctionnalit�s avant de quitter
    static void Shutdown();

    // Activer/d�sactiver les fonctionnalit�s
    static inline bool ESP_Enabled = true;
    static inline bool Aimbot_Enabled = true;
    static inline bool Triggerbot_Enabled = false;
    static inline bool NoRecoil_Enabled = false;

    // Cl�s de touche pour activer les fonctionnalit�s
    static inline int Aimbot_Key = VK_LBUTTON; // Bouton gauche de la souris
    static inline int Triggerbot_Key = VK_XBUTTON1; // Bouton lat�ral de la souris

private:
    // Variables pour les fonctionnalit�s
    static inline float Aimbot_FOV = 5.0f;
    static inline float Aimbot_Smooth = 5.0f;

    // Fonctions pour les fonctionnalit�s
    static void ESP();
    static void Aimbot();
    static void Triggerbot();
    static void NoRecoil();
};
