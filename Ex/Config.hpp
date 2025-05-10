#pragma once
#include <string>
#include "json.hpp" // Assurez-vous que ce chemin est correct

// Si vous utilisez nlohmann::json, utilisez un alias pour simplifier
using json = nlohmann::json;

namespace Config {
    struct AimbotConfig {
        bool enabled = false;
        float fov = 5.0f;
        float smooth = 1.0f;
        bool recoilControl = true;
        bool teamCheck = true;
        int key = 0x01;
        int bone = 0;

        json ToJson() const {
            return {
            {"enabled", enabled},
            {"fov", fov},
            {"smooth", smooth},
            {"recoilControl", recoilControl},
            {"teamCheck", teamCheck},
            {"key", key},
            {"bone", bone},
            };
        }

        void FromJson(const json& j) {
            enabled = j.value("enabled", false);
            fov = j.value("fov", 5.0f);
            smooth = j.value("smooth", 1.0f);
            recoilControl = j.value("recoilControl", true);
            teamCheck = j.value("teamCheck", true);
            key = j.value("key", 0x01);
            bone = j.value("bone", 0);
        }
    };

    struct ESPConfig {
        bool enabled = false;
        bool boxes = true;
        bool health = true;
        bool healthText = false;
        bool name = true;
        bool snaplines = false;
        bool headDot = false;
        bool teamCheck = true;

        json ToJson() const {
            return {
            {"enabled", enabled},
            {"boxes", boxes},
            {"health", health},
            {"healthText", healthText},
            {"name", name},
            {"snaplines", snaplines},
            {"headDot", headDot},
            {"teamCheck", teamCheck}
            };
        }

        void FromJson(const json& j) {
            enabled = j.value("enabled", false);
            boxes = j.value("boxes", true);
            health = j.value("health", true);
            healthText = j.value("healthText", false);
            name = j.value("name", true);
            snaplines = j.value("snaplines", false);
            headDot = j.value("headDot", false);
            teamCheck = j.value("teamCheck", true);
        }
    };

    struct TriggerbotConfig {
        bool enabled = false;
        bool useKey = true;
        int key = 0x12;
        int delay = 10;
        bool teamCheck = true;

        json ToJson() const {
            return {
            {"enabled", enabled},
            {"useKey", useKey},
            {"key", key},
            {"delay", delay},
            {"teamCheck", teamCheck}
            };
        }

        void FromJson(const json& j) {
            enabled = j.value("enabled", false);
            useKey = j.value("useKey", true);
            key = j.value("key", 0x12);
            delay = j.value("delay", 10);
            teamCheck = j.value("teamCheck", true);
        }
    };

    struct NoFlashConfig {
        bool enabled = false;
        float reduction = 255.0f;

        json ToJson() const {
            return {
            {"enabled", enabled},
            {"reduction", reduction}
            };
        }

        void FromJson(const json& j) {
            enabled = j.value("enabled", false);
            reduction = j.value("reduction", 255.0f);
        }
    };

    struct RadarHackConfig {
        bool enabled = false;

        json ToJson() const {
            return {
            {"enabled", enabled}
            };
        }

        void FromJson(const json& j) {
            enabled = j.value("enabled", false);
        }
    };

    struct SkinChangerConfig {
        bool enabled = false;
        // Suppression de la variable redondante skinID

        // Skins spécifiques aux armes
        int ak47Skin = 180;    // Fire Serpent
        int awpSkin = 344;    // Dragon Lore
        int uspSkin = 653;    // Blueprint
        int glockSkin = 38;    // Fade
        int deagleSkin = 527;  // Code Red
        int ssgSkin = 624;    // Bloodshot
        int m4a4Skin = 309;    // Howl
        int m4a1sSkin = 644;   // Printstream

        json ToJson() const {
            return {
                {"enabled", enabled},
                {"ak47Skin", ak47Skin},
                {"awpSkin", awpSkin},
                {"uspSkin", uspSkin},
                {"glockSkin", glockSkin},
                {"deagleSkin", deagleSkin},
                {"ssgSkin", ssgSkin},
                {"m4a4Skin", m4a4Skin},
                {"m4a1sSkin", m4a1sSkin}
            };
        }

        void FromJson(const json& j) {
            enabled = j.value("enabled", false);
            ak47Skin = j.value("ak47Skin", 180);
            awpSkin = j.value("awpSkin", 344);
            uspSkin = j.value("uspSkin", 653);
            glockSkin = j.value("glockSkin", 38);
            deagleSkin = j.value("deagleSkin", 527);
            ssgSkin = j.value("ssgSkin", 624);
            m4a4Skin = j.value("m4a4Skin", 309);
            m4a1sSkin = j.value("m4a1sSkin", 644);
        }
    };

    // Déclarations externes des variables de configuration
    extern AimbotConfig aimbot;
    extern ESPConfig esp;
    extern TriggerbotConfig triggerbot;
    extern NoFlashConfig noFlash;
    extern RadarHackConfig radarHack;
    extern SkinChangerConfig skinChanger;

    // Fonctions pour charger et sauvegarder la configuration
    void Load(const std::string& filename);
    void Save(const std::string& filename);
}