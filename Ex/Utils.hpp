#pragma once
#include <Windows.h>
#include <string>
#include <vector>

// Classe d'utilitaires
class Utils {
public:
    // Convertir un point de la souris en angles d'Euler
    static void MouseToAngles(int mouseX, int mouseY, float& pitch, float& yaw);

    // Convertir des angles d'Euler en un point de la souris
    static void AnglesToMouse(float pitch, float yaw, int& mouseX, int& mouseY);

    // Obtenir la résolution de l'écran
    static void GetScreenResolution(int& width, int& height);

    // Normaliser les angles
    static void NormalizeAngles(float& pitch, float& yaw);

    // Calculer la distance entre deux angles
    static float AngleDistance(float a, float b);

    // Linéariser les angles pour un mouvement fluide
    static float LerpAngle(float a, float b, float factor);

    // Vérifier si un processus est exécuté avec les privilèges d'administrateur
    static bool IsRunningAsAdmin();

    // Obtenir le chemin du fichier exécutable
    static std::string GetExecutablePath();

    // Créer un dossier s'il n'existe pas
    static bool CreateDirectoryIfNotExists(const std::string& path);

    // Sauvegarder la configuration dans un fichier
    static bool SaveConfig(const std::string& filePath, const std::vector<std::pair<std::string, std::string>>& config);

    // Charger la configuration depuis un fichier
    static bool LoadConfig(const std::string& filePath, std::vector<std::pair<std::string, std::string>>& config);
};