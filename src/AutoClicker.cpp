#include "AutoClicker.h"
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <iostream>

AutoClicker::AutoClicker() 
    : display(XOpenDisplay(nullptr)),
      inputHandler(display),
      running(false),
      clicking(false),
      clickInterval(100) {
    
    ConfigManager::Config config;
    configManager.loadConfig(config);
    
    clickInterval = config.clickInterval;
    
    // Set up key bindings
    int toggleKeycode = InputHandler::keySymToKeycode(display, config.toggleKey);
    int holdKeycode = InputHandler::keySymToKeycode(display, config.holdKey);
    
    if (toggleKeycode != 0) {
        inputHandler.setToggleKey(toggleKeycode);
    }
    
    if (holdKeycode != 0) {
        inputHandler.setHoldKey(holdKeycode);
    }
    
    inputHandler.setHoldMode(config.holdModeEnabled);
    
    inputHandler.setToggleCallback([this]() { toggle(); });
    inputHandler.setHoldCallback([this](bool active) { setClicking(active); });
}

AutoClicker::~AutoClicker() {
    stop();
    if (display) XCloseDisplay(display);
}

void AutoClicker::start() {
    if (running) return;
    
    running = true;
    inputHandler.start();
}

void AutoClicker::stop() {
    running = false;
    clicking = false;
    inputHandler.stop();
}

void AutoClicker::toggle() {
    clicking = !clicking;
    if (clicking) {
        std::thread([this]() { clickLoop(); }).detach();
    }
}

void AutoClicker::setClicking(bool active) {
    if (clicking != active) {
        clicking = active;
        if (active) {
            std::thread([this]() { clickLoop(); }).detach();
        }
    }
}

void AutoClicker::setClickInterval(int interval) {
    clickInterval = interval;
}

void AutoClicker::setHoldMode(bool enabled) {
    inputHandler.setHoldMode(enabled);
}

void AutoClicker::updateToggleKey(int keycode) {
    inputHandler.setToggleKey(keycode);
}

void AutoClicker::updateHoldKey(int keycode) {
    inputHandler.setHoldKey(keycode);
}

void AutoClicker::clickLoop() {
    while (clicking) {
        performClick();
        std::this_thread::sleep_for(std::chrono::milliseconds(clickInterval));
    }
}

void AutoClicker::performClick() {
    if (!display) return;
    
    XTestFakeButtonEvent(display, 1, True, CurrentTime);
    XTestFakeButtonEvent(display, 1, False, CurrentTime);
    XFlush(display);
}