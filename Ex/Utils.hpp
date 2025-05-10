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

    // Obtenir la r�solution de l'�cran
    static void GetScreenResolution(int& width, int& height);

    // Normaliser les angles
    static void NormalizeAngles(float& pitch, float& yaw);

    // Calculer la distance entre deux angles
    static float AngleDistance(float a, float b);

    // Lin�ariser les angles pour un mouvement fluide
    static float LerpAngle(float a, float b, float factor);

    // V�rifier si un processus est ex�cut� avec les privil�ges d'administrateur
    static bool IsRunningAsAdmin();

    // Obtenir le chemin du fichier ex�cutable
    static std::string GetExecutablePath();

    // Cr�er un dossier s'il n'existe pas
    static bool CreateDirectoryIfNotExists(const std::string& path);

    // Sauvegarder la configuration dans un fichier
    static bool SaveConfig(const std::string& filePath, const std::vector<std::pair<std::string, std::string>>& config);

    // Charger la configuration depuis un fichier
    static bool LoadConfig(const std::string& filePath, std::vector<std::pair<std::string, std::string>>& config);
};