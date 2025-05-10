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
    static void Init();
    static void Shutdown();
    static void Log(const std::string& message);
    static void Error(const std::string& message);
    static void Debug(const std::string& message);

private:
    static std::string GetTimestamp();
};