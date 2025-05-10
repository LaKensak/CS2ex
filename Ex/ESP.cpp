#include "ESP.hpp"
#include "Offsets.hpp"
#include <sstream>
#include <cmath>

ESP::ESP(Memory& mem) : memory(mem), initialized(false), line(nullptr), font(nullptr), device(nullptr),
windowHandle(nullptr), screenWidth(0), screenHeight(0),
colorT(0, 255, 0), colorCT(0, 0, 255), colorWhite(255, 255, 255),
colorRed(255, 0, 0), colorYellow(255, 255, 0) {
}

ESP::~ESP() {
    Cleanup();
}

bool ESP::Initialize(IDirect3DDevice9* pDevice) {
    if (initialized) return true;

    device = pDevice;

    windowHandle = FindWindowA("Valve001", NULL);
    if (!windowHandle) return false;

    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);
    screenWidth = clientRect.right - clientRect.left;
    screenHeight = clientRect.bottom - clientRect.top;

    if (FAILED(D3DXCreateLine(device, &line))) return false;
    if (FAILED(D3DXCreateFont(device, 14, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &font))) return false;

    initialized = true;
    return true;
}

void ESP::Cleanup() {
    if (line) {
        line->Release();
        line = nullptr;
    }
    if (font) {
        font->Release();
        font = nullptr;
    }
    initialized = false;
}

void ESP::Render(IDirect3DDevice9* pDevice) {
    if (!initialized || !Config::esp.enabled) return;

    uintptr_t localPlayerPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwLocalPlayerController);
    if (!localPlayerPtr) return;

    Entity localPlayer(&memory, localPlayerPtr);
    if (!localPlayer.IsValid() || !localPlayer.IsAlive()) return;

    float viewMatrix[16];
    memory.ReadArray<float>(memory.GetClientBase() + offsets::signatures::dwViewMatrix, viewMatrix, 16);

    for (int i = 1; i < 64; i++) {
        uintptr_t entityPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwEntityList + (i * 0x10));
        if (!entityPtr) continue;

        Entity entity(&memory, entityPtr);
        if (!entity.IsValid() || !entity.IsAlive() || entity.IsDormant()) continue;
        if (entityPtr == localPlayerPtr) continue;

        if (Config::esp.boxes) DrawBox(entity, viewMatrix, entity.IsEnemy(localPlayer));
        if (Config::esp.snaplines) DrawSnapline(entity, viewMatrix, entity.IsEnemy(localPlayer));
        if (Config::esp.health) DrawHealth(entity, viewMatrix);
        if (Config::esp.name) DrawName(entity, viewMatrix, i);
        if (Config::esp.headDot) DrawHeadDot(entity, viewMatrix, entity.IsEnemy(localPlayer));
    }
}

bool ESP::WorldToScreen(const Vector3& pos, Vector2& screen, float* viewMatrix) {
    float w = viewMatrix[3] * pos.x + viewMatrix[7] * pos.y + viewMatrix[11] * pos.z + viewMatrix[15];
    if (w < 0.01f) return false;

    float invw = 1.0f / w;
    screen.x = (viewMatrix[0] * pos.x + viewMatrix[4] * pos.y + viewMatrix[8] * pos.z + viewMatrix[12]) * invw;
    screen.y = (viewMatrix[1] * pos.x + viewMatrix[5] * pos.y + viewMatrix[9] * pos.z + viewMatrix[13]) * invw;

    screen.x = (screen.x * 0.5f + 0.5f) * screenWidth;
    screen.y = (-screen.y * 0.5f + 0.5f) * screenHeight;

    return true;
}

void ESP::DrawBox(const Entity& entity, float* viewMatrix, bool isEnemy) {
    Vector3 origin = entity.GetOrigin();
    Vector3 head = origin;
    head.z += 72.0f;

    Vector2 screenOrigin, screenHead;
    if (!WorldToScreen(origin, screenOrigin, viewMatrix) || !WorldToScreen(head, screenHead, viewMatrix)) return;

    float height = screenOrigin.y - screenHead.y;
    float width = height * 0.5f;

    Color color = isEnemy ? colorRed : colorCT;

    DrawRect(screenHead.x - width / 2, screenHead.y, width, height, color);
}

void ESP::DrawSnapline(const Entity& entity, float* viewMatrix, bool isEnemy) {
    Vector3 origin = entity.GetOrigin();
    Vector2 screenOrigin;
    if (!WorldToScreen(origin, screenOrigin, viewMatrix)) return;

    Color color = isEnemy ? colorRed : colorCT;

    D3DXVECTOR2 lines[2] = {
        D3DXVECTOR2(screenWidth / 2.0f, (float)screenHeight),
        D3DXVECTOR2(screenOrigin.x, screenOrigin.y)
    };

    line->SetWidth(1.0f);
    line->Draw(lines, 2, color.GetD3D());
}

void ESP::DrawHealth(const Entity& entity, float* viewMatrix) {
    Vector3 origin = entity.GetOrigin();
    Vector3 head = origin;
    head.z += 72.0f;

    Vector2 screenOrigin, screenHead;
    if (!WorldToScreen(origin, screenOrigin, viewMatrix) || !WorldToScreen(head, screenHead, viewMatrix)) return;

    float height = screenOrigin.y - screenHead.y;
    float width = height * 0.5f;

    int health = entity.GetHealth();
    float healthRatio = health / 100.0f;
    if (healthRatio > 1.0f) healthRatio = 1.0f;

    Color healthColor;
    healthColor.r = 255 - (int)(255 * healthRatio);
    healthColor.g = (int)(255 * healthRatio);
    healthColor.b = 0;

    float barHeight = height * healthRatio;
    float barWidth = 5.0f;
    float barX = screenHead.x - width / 2 - 10.0f;
    float barY = screenHead.y + height - barHeight;

    DrawFilledRect(barX, screenHead.y, barWidth, height, Color(0, 0, 0, 150));
    DrawFilledRect(barX, barY, barWidth, barHeight, healthColor);

    if (Config::esp.healthText) {
        std::stringstream ss;
        ss << health;
        DrawString(barX, barY - 15, colorWhite, ss.str().c_str());
    }
}

void ESP::DrawName(const Entity& entity, float* viewMatrix, int index) {
    Vector3 head = entity.GetOrigin();
    head.z += 75.0f;

    Vector2 screenHead;
    if (!WorldToScreen(head, screenHead, viewMatrix)) return;

    std::stringstream ss;
    ss << "Player " << index;

    DrawString(screenHead.x, screenHead.y - 15, colorWhite, ss.str().c_str());
}

void ESP::DrawHeadDot(const Entity& entity, float* viewMatrix, bool isEnemy) {
    Vector3 head = entity.GetOrigin();
    head.z += 72.0f;

    Vector2 screenHead;
    if (!WorldToScreen(head, screenHead, viewMatrix)) return;

    Color color = isEnemy ? colorRed : colorCT;

    DrawFilledCircle(screenHead.x, screenHead.y, 3.0f, color);
}

void ESP::DrawRect(float x, float y, float width, float height, const Color& color) {
    D3DXVECTOR2 points[5];
    points[0] = D3DXVECTOR2(x, y);
    points[1] = D3DXVECTOR2(x + width, y);
    points[2] = D3DXVECTOR2(x + width, y + height);
    points[3] = D3DXVECTOR2(x, y + height);
    points[4] = D3DXVECTOR2(x, y);

    line->SetWidth(1.0f);
    line->Draw(points, 5, color.GetD3D());
}

void ESP::DrawFilledRect(float x, float y, float width, float height, const Color& color) {
    D3DRECT rect = { (LONG)x, (LONG)y, (LONG)(x + width), (LONG)(y + height) };
    device->Clear(1, &rect, D3DCLEAR_TARGET, color.GetD3D(), 0.0f, 0);
}

void ESP::DrawFilledCircle(float x, float y, float radius, const Color& color) {
    const int segments = 20;
    D3DXVECTOR2 points[segments + 1];

    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * D3DX_PI * i / segments;
        points[i] = D3DXVECTOR2(x + radius * cos(theta), y + radius * sin(theta));
    }

    line->SetWidth(1.0f);
    line->Draw(points, segments + 1, color.GetD3D());
}

void ESP::DrawString(float x, float y, const Color& color, const char* text) {
    RECT rect = { (LONG)x, (LONG)y, 0, 0 };
    font->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP | DT_LEFT, color.GetD3D());
}
