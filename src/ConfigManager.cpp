#include "ConfigManager.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

ConfigManager::ConfigManager() {
    const char* home = std::getenv("HOME");
    if (home) {
        configDir = std::filesystem::path(home) / ".clickwise";
        configFile = configDir / "config.ini";
    }
}

void ConfigManager::ensureConfigDirectory() {
    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }
}

bool ConfigManager::loadConfig(Config& config) {
    ensureConfigDirectory();
    
    if (!std::filesystem::exists(configFile)) {
        return saveConfig(config); // Create default config
    }
    
    std::ifstream file(configFile);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        size_t delim = line.find('=');
        if (delim == std::string::npos) continue;
        
        std::string key = line.substr(0, delim);
        std::string value = line.substr(delim + 1);
        
        if (key == "click_interval") {
            config.clickInterval = std::stoi(value);
        } else if (key == "toggle_key") {
            config.toggleKey = value;
        } else if (key == "hold_key") {
            config.holdKey = value;
        } else if (key == "hold_mode") {
            config.holdModeEnabled = (value == "true");
        } else if (key == "excluded_app") {
            config.excludedApps.push_back(value);
        }
    }
    
    return true;
}

bool ConfigManager::saveConfig(const Config& config) {
    ensureConfigDirectory();
    
    std::ofstream file(configFile);
    if (!file.is_open()) return false;
    
    file << "# ClickWise Configuration\n";
    file << "click_interval=" << config.clickInterval << "\n";
    file << "toggle_key=" << config.toggleKey << "\n";
    file << "hold_key=" << config.holdKey << "\n";
    file << "hold_mode=" << (config.holdModeEnabled ? "true" : "false") << "\n";
    
    for (const auto& app : config.excludedApps) {
        file << "excluded_app=" << app << "\n";
    }
    
    return true;
}