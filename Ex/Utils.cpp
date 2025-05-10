#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

// Fonction pour convertir un point de la souris en angles d'Euler
void Utils::MouseToAngles(int mouseX, int mouseY, float& pitch, float& yaw) {
    int screenWidth, screenHeight;
    GetScreenResolution(screenWidth, screenHeight);

    // Convertir les coordonnées de la souris en angles
    pitch = ((float)mouseY / (float)screenHeight - 0.5f) * 180.0f;
    yaw = ((float)mouseX / (float)screenWidth - 0.5f) * 360.0f;

    // Normaliser les angles
    NormalizeAngles(pitch, yaw);
}

// Fonction pour convertir des angles d'Euler en un point de la souris
void Utils::AnglesToMouse(float pitch, float yaw, int& mouseX, int& mouseY) {
    int screenWidth, screenHeight;
    GetScreenResolution(screenWidth, screenHeight);

    // Normaliser les angles
    NormalizeAngles(pitch, yaw);

    // Convertir les angles en coordonnées de la souris
    mouseX = (int)((yaw / 360.0f + 0.5f) * screenWidth);
    mouseY = (int)((pitch / 180.0f + 0.5f) * screenHeight);
}

// Fonction pour obtenir la résolution de l'écran
void Utils::GetScreenResolution(int& width, int& height) {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    width = desktop.right;
    height = desktop.bottom;
}

// Fonction pour normaliser les angles
void Utils::NormalizeAngles(float& pitch, float& yaw) {
    // Normaliser le pitch (entre -89 et 89)
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Normaliser le yaw (entre -180 et 180)
    while (yaw > 180.0f) yaw -= 360.0f;
    while (yaw < -180.0f) yaw += 360.0f;
}

// Fonction pour calculer la distance entre deux angles
float Utils::AngleDistance(float a, float b) {
    float delta = b - a;

    // Normaliser la différence
    while (delta > 180.0f) delta -= 360.0f;
    while (delta < -180.0f) delta += 360.0f;

    return abs(delta);
}

// Fonction pour linéariser les angles pour un mouvement fluide
float Utils::LerpAngle(float a, float b, float factor) {
    float delta = b - a;

    // Normaliser la différence
    while (delta > 180.0f) delta -= 360.0f;
    while (delta < -180.0f) delta += 360.0f;

    // Interpolation linéaire
    return a + delta * factor;
}

// Fonction pour vérifier si un processus est exécuté avec les privilèges d'administrateur
bool Utils::IsRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;

    isAdmin = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup);

    if (isAdmin) {
        if (!CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }

    return isAdmin;
}

// Fonction pour obtenir le chemin du fichier exécutable
std::string Utils::GetExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path(buffer);

    // Trouver le dernier séparateur de chemin
    size_t pos = path.find_last_of('\\');
    if (pos != std::string::npos) {
        path = path.substr(0, pos + 1);
    }

    return path;
}

// Fonction pour créer un dossier s'il n'existe pas
bool Utils::CreateDirectoryIfNotExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            return std::filesystem::create_directories(path);
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create directory: " << e.what() << std::endl;
        return false;
    }
}

// Fonction pour sauvegarder la configuration dans un fichier
bool Utils::SaveConfig(const std::string& filePath, const std::vector<std::pair<std::string, std::string>>& config) {
    try {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        for (const auto& [key, value] : config) {
            file << key << "=" << value << std::endl;
        }

        file.close();
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

// Fonction pour charger la configuration depuis un fichier
bool Utils::LoadConfig(const std::string& filePath, std::vector<std::pair<std::string, std::string>>& config) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        config.clear();
        std::string line;

        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            config.push_back({ key, value });
        }

        file.close();
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}
