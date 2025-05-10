#pragma once
#include "Memory.hpp"
#include "SkinChanger.hpp"
#include "Aimbot.hpp"
#include "ESP.hpp"
#include "NoFlash.hpp"
#include "RadarHack.hpp"
#include "Triggerbot.hpp"
#include "Logger.hpp"
#include <thread>
#include <atomic>
#include <chrono>

class CheatManager {
private:
    Memory memory;
    ESP esp;
    Aimbot aimbot;
    RadarHack radarHack;
    Triggerbot triggerbot;
    NoFlash noFlash;
    SkinChanger skinChanger;
    std::thread cheatThread;
    std::atomic<bool> running;
    IDirect3DDevice9* pDevice;

    void CheatLoop() {
        while (running) {
            RunFeatures();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

public:
    CheatManager() :
        memory(),
        esp(memory),
        aimbot(memory),
        radarHack(memory),
        triggerbot(memory),
        noFlash(memory),
        skinChanger(memory),
        running(false),
        pDevice(nullptr)
    {
    }

    ~CheatManager() {
        Stop();
    }

    void SetDevice(IDirect3DDevice9* device) {
        pDevice = device;
        if (pDevice) {
            esp.Initialize(pDevice);
        }
    }

    bool Initialize() {
        Logger::Init();
        Logger::Log("Initializing cheat...");

        if (!memory.Initialize(L"cs2.exe", L"client.dll", L"engine2.dll")) {
            Logger::Error("Failed to initialize memory");
            return false;
        }
        Logger::Log("Memory initialized");

        Config::Load("config.json");
        Logger::Log("Configuration loaded");

        if (!aimbot.Initialize()) {
            Logger::Error("Failed to initialize aimbot");
            return false;
        }

        // ESP est initialisé dans SetDevice

        if (!radarHack.Initialize()) {
            Logger::Error("Failed to initialize radar hack");
            return false;
        }

        if (!triggerbot.Initialize()) {
            Logger::Error("Failed to initialize triggerbot");
            return false;
        }

        if (!noFlash.Initialize()) {
            Logger::Error("Failed to initialize no flash");
            return false;
        }

        if (!skinChanger.Initialize()) {
            Logger::Error("Failed to initialize skin changer");
            return false;
        }

        return true;
    }

    void RunFeatures() {
        if (Config::aimbot.enabled)
            aimbot.Run();

        // ESP est rendu dans le hook DirectX, pas ici

        if (Config::triggerbot.enabled)
            triggerbot.Run();

        if (Config::noFlash.enabled)
            noFlash.Run();

        if (Config::radarHack.enabled)
            radarHack.Run();

        if (Config::skinChanger.enabled)
            skinChanger.Run();
    }

    void Start() {
        if (running) return;
        running = true;
        cheatThread = std::thread(&CheatManager::CheatLoop, this);
    }

    void Stop() {
        if (!running) return;
        running = false;
        if (cheatThread.joinable())
            cheatThread.join();
    }

    void RenderESP() {
        if (pDevice && Config::esp.enabled) {
            esp.Render(pDevice);
        }
    }
};