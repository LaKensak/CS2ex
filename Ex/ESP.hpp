#pragma once
#include "Memory.hpp"
#include "Entity.hpp"
#include "Vector.hpp"
#include "Config.hpp"
#include <d3d9.h>
#include <d3dx9.h>

class ESP {
private:
    Memory& memory;
    bool initialized;
    ID3DXLine* line;
    ID3DXFont* font;
    IDirect3DDevice9* device;
    HWND windowHandle;
    int screenWidth;
    int screenHeight;

    struct Color {
        int r, g, b, a;
        Color() : r(255), g(255), b(255), a(255) {}
        Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
        D3DCOLOR GetD3D() const { return D3DCOLOR_RGBA(r, g, b, a); }
    };

    const Color colorT;
    const Color colorCT;
    const Color colorWhite;
    const Color colorRed;
    const Color colorYellow;

public:
    ESP(Memory& mem);
    ~ESP();

    bool Initialize(IDirect3DDevice9* device);
    void Cleanup();
    void Render(IDirect3DDevice9* device);

private:
    bool WorldToScreen(const Vector3& pos, Vector2& screen, float* viewMatrix);
    void DrawBox(const Entity& entity, float* viewMatrix, bool isEnemy);
    void DrawSnapline(const Entity& entity, float* viewMatrix, bool isEnemy);
    void DrawHealth(const Entity& entity, float* viewMatrix);
    void DrawName(const Entity& entity, float* viewMatrix, int index);
    void DrawHeadDot(const Entity& entity, float* viewMatrix, bool isEnemy);

    void DrawRect(float x, float y, float width, float height, const Color& color);
    void DrawFilledRect(float x, float y, float width, float height, const Color& color);
    void DrawFilledCircle(float x, float y, float radius, const Color& color);
    void DrawString(float x, float y, const Color& color, const char* text);
};
