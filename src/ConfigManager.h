#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <filesystem>

class ConfigManager {
private:
    std::filesystem::path configDir;
    std::filesystem::path configFile;
    
    void ensureConfigDirectory();
    
public:
    struct Config {
        int clickInterval = 100;
        std::string toggleKey = "F6";
        std::string holdKey = "Caps_Lock";
        bool holdModeEnabled = false;
        std::vector<std::string> excludedApps;
    };
    
    ConfigManager();
    bool loadConfig(Config& config);
    bool saveConfig(const Config& config);
    std::string getConfigPath() const;
};

#endif