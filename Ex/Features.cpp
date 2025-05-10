#include "Features.hpp"
#include "Memory.hpp"
#include <iostream>
#include <cmath>

// Structures pour CS2
struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // Op�rateurs utiles
    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }
};

struct Matrix4x4 {
    float m[4][4];
};

// Offsets pour CS2 (� mettre � jour r�guli�rement car ils peuvent changer)
namespace Offsets {
    // Offsets du joueur local
    constexpr auto LocalPlayer_Origin = 0x138;
    constexpr auto LocalPlayer_ViewOffset = 0x108;
    constexpr auto LocalPlayer_Health = 0x32C;
    constexpr auto LocalPlayer_Team = 0x3BF;
    constexpr auto LocalPlayer_AimPunch = 0x171C;

    // Offsets de l'entit�
    constexpr auto Entity_Origin = 0x138;
    constexpr auto Entity_Health = 0x32C;
    constexpr auto Entity_Team = 0x3BF;
    constexpr auto Entity_Dormant = 0x125;

    // Nombre maximum d'entit�s
    constexpr int MaxEntities = 64;
}

// Fonction pour v�rifier si un point est visible � l'�cran
bool WorldToScreen(const Vector3& worldPos, Vector3& screenPos) {
    // Obtenir la matrice de vue (WorldToScreen)
    const Matrix4x4& viewMatrix = Memory::Read<Matrix4x4>((uintptr_t)Memory::ViewMatrix);

    // Multiplier la position du monde avec la matrice de vue
    float w = viewMatrix.m[3][0] * worldPos.x + viewMatrix.m[3][1] * worldPos.y + viewMatrix.m[3][2] * worldPos.z + viewMatrix.m[3][3];

    // V�rifier si le point est devant la cam�ra
    if (w < 0.01f)
        return false;

    // Obtenir les dimensions de l'�cran
    RECT rect;
    HWND hwnd = FindWindowA(NULL, "Counter-Strike 2");
    if (!hwnd)
        return false;

    GetClientRect(hwnd, &rect);
    int screenWidth = rect.right - rect.left;
    int screenHeight = rect.bottom - rect.top;

    // Calculer les coordonn�es de l'�cran
    float invW = 1.0f / w;
    screenPos.x = (viewMatrix.m[0][0] * worldPos.x + viewMatrix.m[0][1] * worldPos.y + viewMatrix.m[0][2] * worldPos.z + viewMatrix.m[0][3]) * invW;
    screenPos.y = (viewMatrix.m[1][0] * worldPos.x + viewMatrix.m[1][1] * worldPos.y + viewMatrix.m[1][2] * worldPos.z + viewMatrix.m[1][3]) * invW;

    // Ajuster pour l'�cran
    screenPos.x = (screenWidth / 2.0f) * (1.0f + screenPos.x);
    screenPos.y = (screenHeight / 2.0f) * (1.0f - screenPos.y);

    return true;
}

// Fonction pour initialiser les fonctionnalit�s
bool Features::Initialize() {
    try {
        std::cout << "Initializing features..." << std::endl;

        // V�rifier si les pointeurs n�cessaires sont valides
        if (!Memory::LocalPlayer || !Memory::EntityList || !Memory::ViewMatrix) {
            std::cout << "Required memory pointers are not valid!" << std::endl;
            return false;
        }

        std::cout << "Features initialized successfully!" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Exception during features initialization: " << e.what() << std::endl;
        return false;
    }
}

// Fonction pour mettre � jour les fonctionnalit�s
void Features::Update() {
    // Ex�cuter les fonctionnalit�s activ�es
    if (ESP_Enabled) {
        ESP();
    }

    if (Aimbot_Enabled && (GetAsyncKeyState(Aimbot_Key) & 0x8000)) {
        Aimbot();
    }

    if (Triggerbot_Enabled && (GetAsyncKeyState(Triggerbot_Key) & 0x8000)) {
        Triggerbot();
    }

    if (NoRecoil_Enabled) {
        NoRecoil();
    }
}

// Fonction pour arr�ter les fonctionnalit�s
void Features::Shutdown() {
    // Nettoyer les ressources si n�cessaire
    std::cout << "Shutting down features..." << std::endl;
}

// Fonction pour l'ESP (Extra Sensory Perception)
void Features::ESP() {
    // Obtenir le joueur local
    uintptr_t localPlayerPtr = Memory::Read<uintptr_t>((uintptr_t)Memory::LocalPlayer);
    if (!localPlayerPtr) {
        return;
    }

    // Obtenir l'�quipe du joueur local
    int localTeam = Memory::Read<int>(localPlayerPtr + Offsets::LocalPlayer_Team);

    // Pour chaque entit� (joueur) dans le jeu
    for (int i = 1; i < Offsets::MaxEntities; i++) {
        // Obtenir l'entit�
        uintptr_t entityListPtr = Memory::Read<uintptr_t>((uintptr_t)Memory::EntityList);
        uintptr_t entityPtr = Memory::Read<uintptr_t>(entityListPtr + (i * 0x8));

        if (!entityPtr) {
            continue;
        }

        // V�rifier si l'entit� est en sommeil (non visible)
        bool isDormant = Memory::Read<bool>(entityPtr + Offsets::Entity_Dormant);
        if (isDormant) {
            continue;
        }

        // V�rifier si l'entit� est en vie
        // V�rifier si l'entit� est en vie
        int health = Memory::Read<int>(entityPtr + Offsets::Entity_Health);
        if (health <= 0) {
            continue;
        }

        // V�rifier si l'entit� est dans l'�quipe du joueur
        int team = Memory::Read<int>(entityPtr + Offsets::Entity_Team);
        if (team == localTeam) {
            // Ignorer les co�quipiers (ou ajuster selon vos pr�f�rences)
            continue;
        }

        // Obtenir la position de l'entit�
        Vector3 entityPos = Memory::Read<Vector3>(entityPtr + Offsets::Entity_Origin);

        // Convertir la position du monde en coordonn�es d'�cran
        Vector3 screenPos;
        if (WorldToScreen(entityPos, screenPos)) {
            // Ici, vous pouvez dessiner un ESP (box, ligne, etc.)
            // Note: Le dessin n�cessite g�n�ralement des hooks DirectX ou GDI
            // qui ne sont pas inclus dans ce code de base

            // Pour l'instant, nous allons simplement enregistrer les positions
            // std::cout << "Player " << i << " at screen pos: (" << screenPos.x << ", " << screenPos.y << ")" << std::endl;
        }
    }
}

// Fonction pour l'Aimbot
void Features::Aimbot() {
    // Obtenir le joueur local
    uintptr_t localPlayerPtr = Memory::Read<uintptr_t>((uintptr_t)Memory::LocalPlayer);
    if (!localPlayerPtr) {
        return;
    }

    // Obtenir la position de la t�te du joueur local
    Vector3 localPos = Memory::Read<Vector3>(localPlayerPtr + Offsets::LocalPlayer_Origin);
    Vector3 viewOffset = Memory::Read<Vector3>(localPlayerPtr + Offsets::LocalPlayer_ViewOffset);
    Vector3 localEyePos = localPos + viewOffset;

    // Obtenir l'�quipe du joueur local
    int localTeam = Memory::Read<int>(localPlayerPtr + Offsets::LocalPlayer_Team);

    // Variables pour trouver la cible la plus proche
    float closestDistance = FLT_MAX;
    Vector3 aimAngle;
    bool targetFound = false;

    // Pour chaque entit� (joueur) dans le jeu
    for (int i = 1; i < Offsets::MaxEntities; i++) {
        // Obtenir l'entit�
        uintptr_t entityListPtr = Memory::Read<uintptr_t>((uintptr_t)Memory::EntityList);
        uintptr_t entityPtr = Memory::Read<uintptr_t>(entityListPtr + (i * 0x8));

        if (!entityPtr) {
            continue;
        }

        // V�rifier si l'entit� est en sommeil (non visible)
        bool isDormant = Memory::Read<bool>(entityPtr + Offsets::Entity_Dormant);
        if (isDormant) {
            continue;
        }

        // V�rifier si l'entit� est en vie
        int health = Memory::Read<int>(entityPtr + Offsets::Entity_Health);
        if (health <= 0) {
            continue;
        }

        // V�rifier si l'entit� est dans l'�quipe du joueur
        int team = Memory::Read<int>(entityPtr + Offsets::Entity_Team);
        if (team == localTeam) {
            continue;
        }

        // Obtenir la position de la t�te de l'entit� (approximative)
        Vector3 entityPos = Memory::Read<Vector3>(entityPtr + Offsets::Entity_Origin);
        entityPos.z += 70.0f; // Approximation de la position de la t�te

        // Calculer la direction vers la cible
        Vector3 direction = entityPos - localEyePos;
        float distance = direction.Length();

        // Convertir la direction en angles (pitch, yaw)
        float pitch = -asinf(direction.z / distance) * (180.0f / 3.14159f);
        float yaw = atan2f(direction.y, direction.x) * (180.0f / 3.14159f);

        // V�rifier si la cible est dans le FOV de l'aimbot
        // (N�cessite d'obtenir les angles actuels de la vue, non inclus ici)
        // Pour simplifier, nous allons simplement prendre la cible la plus proche
        if (distance < closestDistance) {
            closestDistance = distance;
            aimAngle = Vector3(pitch, yaw, 0.0f);
            targetFound = true;
        }
    }

    // Si une cible a �t� trouv�e, ajuster la vis�e
    if (targetFound) {
        // Ici, vous devriez avoir des hooks pour modifier les angles de vue
        // ou simuler un mouvement de souris vers la cible
        // Pour l'instant, nous allons simplement enregistrer l'angle
        // std::cout << "Aim at: (" << aimAngle.x << ", " << aimAngle.y << ")" << std::endl;

        // Exemple d'impl�mentation pour simuler un mouvement de souris (non fonctionnel sans les bons hooks)
        // ApplyAimAngles(aimAngle, Aimbot_Smooth);
    }
}

// Fonction pour le Triggerbot
void Features::Triggerbot() {
    // Obtenir le joueur local
    uintptr_t localPlayerPtr = Memory::Read<uintptr_t>((uintptr_t)Memory::LocalPlayer);
    if (!localPlayerPtr) {
        return;
    }

    // Ici, vous devriez v�rifier si la vis�e du joueur est sur un ennemi
    // et simuler un clic de souris
    // N�cessite des hooks sp�cifiques non inclus ici

    // Exemple simple (non fonctionnel sans les bons hooks):
    // if (IsEnemyInCrosshair()) {
    //     mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    //     Sleep(10);
    //     mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    // }
}

// Fonction pour le NoRecoil
void Features::NoRecoil() {
    // Obtenir le joueur local
    uintptr_t localPlayerPtr = Memory::Read<uintptr_t>((uintptr_t)Memory::LocalPlayer);
    if (!localPlayerPtr) {
        return;
    }

    // Obtenir les angles de recul (punch angles)
    Vector3 aimPunch = Memory::Read<Vector3>(localPlayerPtr + Offsets::LocalPlayer_AimPunch);

    // Ici, vous devriez avoir des hooks pour modifier les angles de vue
    // et compenser le recul
    // Pour l'instant, nous allons simplement enregistrer les angles de recul
    // std::cout << "Recoil: (" << aimPunch.x << ", " << aimPunch.y << ")" << std::endl;

    // Exemple d'impl�mentation (non fonctionnel sans les bons hooks):
    // CompensateRecoil(aimPunch);
}