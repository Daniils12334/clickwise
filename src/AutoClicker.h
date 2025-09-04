#ifndef AUTOCLICKER_H
#define AUTOCLICKER_H

#include "InputHandler.h"
#include "ConfigManager.h"
#include <thread>
#include <atomic>

class AutoClicker {
private:
    Display* display;
    ConfigManager configManager;
    InputHandler inputHandler;
    
    std::atomic<bool> running;
    std::atomic<bool> clicking;
    std::atomic<int> clickInterval;
    std::thread clickThread;
    
    void clickLoop();
    void performClick();
    
public:
    AutoClicker();
    ~AutoClicker();
    
    void start();
    void stop();
    void toggle();
    void setClicking(bool active);
    
    void setClickInterval(int interval);
    void setHoldMode(bool enabled);
    void updateToggleKey(int keycode);
    void updateHoldKey(int keycode);
    
    bool isClicking() const { return clicking; }
    int getClickInterval() const { return clickInterval; }
};

#endif