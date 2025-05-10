#pragma once
#include "Memory.hpp"
#include "SkinChanger.hpp"
#include "Aimbot.hpp"
#include "ESP.hpp"
#include "NoFlash.hpp"
#include "RadarHack.hpp"
#include "Triggerbot.hpp"
#include "Logger.hpp" 
#include <d3d9.h>

class CheatManager {
private:
    Memory memory;
    SkinChanger skinChanger;
    Aimbot aimbot;
    ESP esp;
    NoFlash noFlash;
    RadarHack radarHack;
    Triggerbot triggerbot;
    IDirect3DDevice9* pDevice;
    bool running;

public:
    CheatManager() :
        memory(),
        skinChanger(memory),
        aimbot(memory),
        esp(memory),
        noFlash(memory),
        radarHack(memory),
        triggerbot(memory),
        pDevice(nullptr),
        running(false) {
    }

    void SetDevice(IDirect3DDevice9* device) {
        pDevice = device;
    }

    bool Initialize() {
        if (!memory.Initialize(L"cs2.exe")) {
            Logger::Error("Failed to initialize Memory");
            return false;
        }

        // Initialiser ESP avec le device
        if (pDevice && !esp.Initialize(pDevice)) {
            Logger::Error("Failed to initialize ESP");
            return false;
        }

        return true;
    }

    void Start() {
        running = true;
    }

    void Stop() {
        running = false;
    }

    void RunFeatures() {
        if (!running) return;

        // Exécuter les fonctionnalités qui ne nécessitent pas de rendu
        if (Config::skinChanger.enabled) skinChanger.Run();
        if (Config::aimbot.enabled) aimbot.Run();
        if (Config::noFlash.enabled) noFlash.Run();
        if (Config::radarHack.enabled) radarHack.Run();
        if (Config::triggerbot.enabled) triggerbot.Run();

        // Exécuter les fonctionnalités qui nécessitent un rendu
        if (Config::esp.enabled && pDevice) {
            esp.Render(pDevice);
        }
    }
};