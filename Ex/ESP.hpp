// ESP.hpp
#pragma once
#include "Memory.hpp"
#include "Vector.hpp"
#include "Entity.hpp"
#include "Config.hpp"
#include <d3d9.h>
#include <d3dx9.h>

class ESP {
private:
    Memory& memory;
    ID3DXLine* line;
    ID3DXFont* font;
    bool initialized;

    bool WorldToScreen(const Vector3& world, Vector2& screen);

public:
    ESP(Memory& mem) : memory(mem), line(nullptr), font(nullptr), initialized(false) {}
    ~ESP() {
        if (line) line->Release();
        if (font) font->Release();
    }

    bool Initialize(IDirect3DDevice9* device);
    void Render(IDirect3DDevice9* device);
    void DrawBox(IDirect3DDevice9* device, float x, float y, float width, float height, D3DCOLOR color);
    void DrawLine(IDirect3DDevice9* device, float x1, float y1, float x2, float y2, D3DCOLOR color);
    void DrawText(IDirect3DDevice9* device, const char* text, float x, float y, D3DCOLOR color);
};