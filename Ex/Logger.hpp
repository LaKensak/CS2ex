#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <ctime>

class Logger {
private:
    static std::ofstream logFile;
    static bool consoleAllocated;

public:
    static void Init() {
        // Allouer une console
        if (!consoleAllocated) {
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            consoleAllocated = true;
        }

        // Ouvrir le fichier de log
        logFile.open("csgo_cheat.log", std::ios::out | std::ios::app);
    }

    static void Shutdown() {
        if (logFile.is_open()) {
            logFile.close();
        }

        if (consoleAllocated) {
            FreeConsole();
            consoleAllocated = false;
        }
    }

    static void Log(const std::string& message) {
        std::string timestamp = GetTimestamp();
        std::string formattedMessage = timestamp + " [INFO] " + message;

        std::cout << formattedMessage << std::endl;

        if (logFile.is_open()) {
            logFile << formattedMessage << std::endl;
        }
    }

    static void Error(const std::string& message) {
        std::string timestamp = GetTimestamp();
        std::string formattedMessage = timestamp + " [ERROR] " + message;

        std::cout << formattedMessage << std::endl;

        if (logFile.is_open()) {
            logFile << formattedMessage << std::endl;
        }
    }

    static void Debug(const std::string& message) {
#ifdef _DEBUG
        std::string timestamp = GetTimestamp();
        std::string formattedMessage = timestamp + " [DEBUG] " + message;

        std::cout << formattedMessage << std::endl;

        if (logFile.is_open()) {
            logFile << formattedMessage << std::endl;
        }
#endif
    }

private:
    static std::string GetTimestamp() {
        time_t now = time(0);
        struct tm timeinfo;
        char buffer[80];

        localtime_s(&timeinfo, &now);
        strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &timeinfo);

        return std::string(buffer);
    }
};

// Initialisation des variables statiques
std::ofstream Logger::logFile;
bool Logger::consoleAllocated = false;