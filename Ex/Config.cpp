#include "Config.hpp"
#include <fstream>
#include "json.hpp"

// Définition des variables globales
Config::AimbotConfig Config::aimbot;
Config::ESPConfig Config::esp;
Config::TriggerbotConfig Config::triggerbot;
Config::NoFlashConfig Config::noFlash;
Config::RadarHackConfig Config::radarHack;
Config::SkinChangerConfig Config::skinChanger;

void Config::Save(const std::string& filename) {
    nlohmann::json j;

    // Aimbot
    j["aimbot"]["enabled"] = aimbot.enabled;
    j["aimbot"]["fov"] = aimbot.fov;
    j["aimbot"]["smooth"] = aimbot.smooth;
    j["aimbot"]["recoilControl"] = aimbot.recoilControl;
    j["aimbot"]["teamCheck"] = aimbot.teamCheck;
    j["aimbot"]["key"] = aimbot.key;
    j["aimbot"]["bone"] = aimbot.bone;

    // ESP
    j["esp"]["enabled"] = esp.enabled;
    j["esp"]["boxes"] = esp.boxes;
    j["esp"]["health"] = esp.health;
    j["esp"]["healthText"] = esp.healthText;
    j["esp"]["name"] = esp.name;
    j["esp"]["snaplines"] = esp.snaplines;
    j["esp"]["headDot"] = esp.headDot;
    j["esp"]["teamCheck"] = esp.teamCheck;

    // Triggerbot
    j["triggerbot"]["enabled"] = triggerbot.enabled;
    j["triggerbot"]["useKey"] = triggerbot.useKey;
    j["triggerbot"]["key"] = triggerbot.key;
    j["triggerbot"]["delay"] = triggerbot.delay;
    j["triggerbot"]["teamCheck"] = triggerbot.teamCheck;

    // NoFlash
    j["noFlash"]["enabled"] = noFlash.enabled;
    j["noFlash"]["reduction"] = noFlash.reduction;

    // RadarHack
    j["radarHack"]["enabled"] = radarHack.enabled;

    // SkinChanger
    j["skinChanger"]["enabled"] = skinChanger.enabled;
    j["skinChanger"]["ak47Skin"] = skinChanger.ak47Skin;
    j["skinChanger"]["awpSkin"] = skinChanger.awpSkin;
    j["skinChanger"]["uspSkin"] = skinChanger.uspSkin;
    j["skinChanger"]["glockSkin"] = skinChanger.glockSkin;
    j["skinChanger"]["deagleSkin"] = skinChanger.deagleSkin;
    j["skinChanger"]["ssgSkin"] = skinChanger.ssgSkin;
    j["skinChanger"]["m4a4Skin"] = skinChanger.m4a4Skin;
    j["skinChanger"]["m4a1sSkin"] = skinChanger.m4a1sSkin;

    // Écrire dans le fichier
    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4); // Indentation de 4 espaces pour la lisibilité
        file.close();
    }
}

void Config::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    try {
        nlohmann::json j;
        file >> j;
        file.close();

        // Aimbot
        if (j.contains("aimbot")) {
            aimbot.enabled = j["aimbot"].value("enabled", aimbot.enabled);
            aimbot.fov = j["aimbot"].value("fov", aimbot.fov);
            aimbot.smooth = j["aimbot"].value("smooth", aimbot.smooth);
            aimbot.recoilControl = j["aimbot"].value("recoilControl", aimbot.recoilControl);
            aimbot.teamCheck = j["aimbot"].value("teamCheck", aimbot.teamCheck);
            aimbot.key = j["aimbot"].value("key", aimbot.key);
            aimbot.bone = j["aimbot"].value("bone", aimbot.bone);
        }

        // ESP
        if (j.contains("esp")) {
            esp.enabled = j["esp"].value("enabled", esp.enabled);
            esp.boxes = j["esp"].value("boxes", esp.boxes);
            esp.health = j["esp"].value("health", esp.health);
            esp.healthText = j["esp"].value("healthText", esp.healthText);
            esp.name = j["esp"].value("name", esp.name);
            esp.snaplines = j["esp"].value("snaplines", esp.snaplines);
            esp.headDot = j["esp"].value("headDot", esp.headDot);
            esp.teamCheck = j["esp"].value("teamCheck", esp.teamCheck);
        }

        // Triggerbot
        if (j.contains("triggerbot")) {
            triggerbot.enabled = j["triggerbot"].value("enabled", triggerbot.enabled);
            triggerbot.useKey = j["triggerbot"].value("useKey", triggerbot.useKey);
            triggerbot.key = j["triggerbot"].value("key", triggerbot.key);
            triggerbot.delay = j["triggerbot"].value("delay", triggerbot.delay);
            triggerbot.teamCheck = j["triggerbot"].value("teamCheck", triggerbot.teamCheck);
        }

        // NoFlash
        if (j.contains("noFlash")) {
            noFlash.enabled = j["noFlash"].value("enabled", noFlash.enabled);
            noFlash.reduction = j["noFlash"].value("reduction", noFlash.reduction);
        }

        // RadarHack
        if (j.contains("radarHack")) {
            radarHack.enabled = j["radarHack"].value("enabled", radarHack.enabled);
        }

        // SkinChanger
        if (j.contains("skinChanger")) {
            skinChanger.enabled = j["skinChanger"].value("enabled", skinChanger.enabled);
            skinChanger.ak47Skin = j["skinChanger"].value("ak47Skin", skinChanger.ak47Skin);
            skinChanger.awpSkin = j["skinChanger"].value("awpSkin", skinChanger.awpSkin);
            skinChanger.uspSkin = j["skinChanger"].value("uspSkin", skinChanger.uspSkin);
            skinChanger.glockSkin = j["skinChanger"].value("glockSkin", skinChanger.glockSkin);
            skinChanger.deagleSkin = j["skinChanger"].value("deagleSkin", skinChanger.deagleSkin);
            skinChanger.ssgSkin = j["skinChanger"].value("ssgSkin", skinChanger.ssgSkin);
            skinChanger.m4a4Skin = j["skinChanger"].value("m4a4Skin", skinChanger.m4a4Skin);
            skinChanger.m4a1sSkin = j["skinChanger"].value("m4a1sSkin", skinChanger.m4a1sSkin);
        }
    }
    catch (const std::exception& e) {
        // Gérer les erreurs de parsing JSON
    }
}