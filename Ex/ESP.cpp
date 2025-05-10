// ESP.cpp
#include "ESP.hpp"
#include "Logger.hpp"
#include "Offsets.hpp"

bool ESP::Initialize(IDirect3DDevice9* device) {
    if (!device) return false;

    // Créer les ressources DirectX
    if (FAILED(D3DXCreateLine(device, &line))) {
        Logger::Error("Failed to create D3DX line");
        return false;
    }

    if (FAILED(D3DXCreateFont(device, 14, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &font))) {
        if (line) {
            line->Release();
            line = nullptr;
        }
        Logger::Error("Failed to create D3DX font");
        return false;
    }

    initialized = true;
    Logger::Log("ESP initialized successfully");
    return true;
}

bool ESP::WorldToScreen(const Vector3& world, Vector2& screen) {
    // Obtenir la matrice de vue
    const uintptr_t viewMatrix = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwViewMatrix);
    if (!memory.IsValidPointer(viewMatrix)) return false;

    // Lire la matrice
    float matrix[16];
    for (int i = 0; i < 16; i++) {
        matrix[i] = memory.Read<float>(viewMatrix + i * sizeof(float));
    }

    // Calculer les coordonnées d'écran
    float w = matrix[3] * world.x + matrix[7] * world.y + matrix[11] * world.z + matrix[15];
    if (w < 0.01f) return false;

    // Obtenir les dimensions de l'écran
    RECT rect;
    GetClientRect(GetForegroundWindow(), &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Calculer les coordonnées 2D
    float x = matrix[0] * world.x + matrix[4] * world.y + matrix[8] * world.z + matrix[12];
    float y = matrix[1] * world.x + matrix[5] * world.y + matrix[9] * world.z + matrix[13];

    screen.x = (width / 2) * (1 + x / w);
    screen.y = (height / 2) * (1 - y / w);

    return true;
}

void ESP::Render(IDirect3DDevice9* device) {
    if (!initialized || !device || !Config::esp.enabled) return;

    // Obtenir le joueur local
    const uintptr_t localPlayer = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwLocalPlayerPawn);
    if (!memory.IsValidPointer(localPlayer)) return;

    // Obtenir l'équipe du joueur local
    const int localTeam = memory.Read<int>(localPlayer + offsets::netvars::m_iTeamNum);

    // Parcourir la liste des entités
    for (int i = 1; i < 64; i++) {
        const uintptr_t entityList = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwEntityList);
        if (!memory.IsValidPointer(entityList)) continue;

        const uintptr_t entity = memory.Read<uintptr_t>(entityList + (i * 0x78));
        if (!memory.IsValidPointer(entity)) continue;

        // Vérifier si l'entité est un joueur
        const int health = memory.Read<int>(entity + offsets::netvars::m_iHealth);
        if (health <= 0 || health > 100) continue;

        // Vérifier l'équipe
        const int team = memory.Read<int>(entity + offsets::netvars::m_iTeamNum);
        if (Config::esp.teamCheck && team == localTeam) continue;

        // Obtenir la position
        Vector3 position = memory.Read<Vector3>(entity + offsets::netvars::m_vecOrigin);
        position.z += 64.0f; // Ajuster pour la hauteur du joueur

        // Convertir en coordonnées d'écran
        Vector2 screenPos;
        if (!WorldToScreen(position, screenPos)) continue;

        // Dessiner l'ESP
        if (Config::esp.boxes) {
            DrawBox(device, screenPos.x - 20, screenPos.y - 40, 40, 80, D3DCOLOR_RGBA(255, 0, 0, 255));
        }

        if (Config::esp.health) {
            // Dessiner la barre de vie
            float healthPercent = health / 100.0f;
            DrawBox(device, screenPos.x - 25, screenPos.y - 40, 5, 80, D3DCOLOR_RGBA(0, 0, 0, 255));
            DrawBox(device, screenPos.x - 25, screenPos.y - 40 + (80 * (1 - healthPercent)), 5, 80 * healthPercent, D3DCOLOR_RGBA(0, 255, 0, 255));
        }

        if (Config::esp.name) {
            // Dessiner le nom (ici simplement "Player")
            DrawText(device, "Player", screenPos.x, screenPos.y - 55, D3DCOLOR_RGBA(255, 255, 255, 255));
        }

        if (Config::esp.snaplines) {
            // Dessiner une ligne vers le joueur
            RECT rect;
            GetClientRect(GetForegroundWindow(), &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            DrawLine(device, width / 2, height, screenPos.x, screenPos.y, D3DCOLOR_RGBA(255, 0, 0, 255));
        }
    }
}

void ESP::DrawBox(IDirect3DDevice9* device, float x, float y, float width, float height, D3DCOLOR color) {
    if (!line) return;

    D3DXVECTOR2 points[5];
    points[0] = D3DXVECTOR2(x, y);
    points[1] = D3DXVECTOR2(x + width, y);
    points[2] = D3DXVECTOR2(x + width, y + height);
    points[3] = D3DXVECTOR2(x, y + height);
    points[4] = D3DXVECTOR2(x, y);

    line->SetWidth(1.0f);
    line->Draw(points, 5, color);
}

void ESP::DrawLine(IDirect3DDevice9* device, float x1, float y1, float x2, float y2, D3DCOLOR color) {
    if (!line) return;

    D3DXVECTOR2 points[2];
    points[0] = D3DXVECTOR2(x1, y1);
    points[1] = D3DXVECTOR2(x2, y2);

    line->SetWidth(1.0f);
    line->Draw(points, 2, color);
}

void ESP::DrawText(IDirect3DDevice9* device, const char* text, float x, float y, D3DCOLOR color) {
    if (!font) return;

    RECT rect = { (LONG)x, (LONG)y, 0, 0 };
    font->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
}