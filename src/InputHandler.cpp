#include "InputHandler.h"
#include <X11/keysym.h>
#include <iostream>
#include <unistd.h>

InputHandler::InputHandler(Display* dpy) : display(dpy), holdModeEnabled(false), running(false) {
    toggleKeycode = XKeysymToKeycode(display, XStringToKeysym("F6"));
    holdKeycode = XKeysymToKeycode(display, XStringToKeysym("Caps_Lock"));
}

InputHandler::~InputHandler() {
    stop();
}

void InputHandler::setToggleKey(int keycode) {
    toggleKeycode = keycode;
}

void InputHandler::setHoldKey(int keycode) {
    holdKeycode = keycode;
}

void InputHandler::setHoldMode(bool enabled) {
    holdModeEnabled = enabled;
}

void InputHandler::setToggleCallback(std::function<void()> callback) {
    toggleCallback = callback;
}

void InputHandler::setHoldCallback(std::function<void(bool)> callback) {
    holdCallback = callback;
}

bool InputHandler::isKeyPressed(int keycode) {
    if (keycode == 0) return false;
    
    char keys[32];
    XQueryKeymap(display, keys);
    return (keys[keycode / 8] & (1 << (keycode % 8))) != 0;
}

void InputHandler::inputLoop() {
    static bool toggleWasPressed = false;
    static bool holdWasPressed = false;
    
    while (running) {
        bool togglePressed = isKeyPressed(toggleKeycode);
        bool holdPressed = isKeyPressed(holdKeycode);
        
        // Toggle mode
        if (togglePressed && !toggleWasPressed && toggleCallback) {
            toggleCallback();
        }
        
        // Hold mode
        if (holdModeEnabled) {
            if (holdPressed != holdWasPressed && holdCallback) {
                holdCallback(holdPressed);
            }
        }
        
        toggleWasPressed = togglePressed;
        holdWasPressed = holdPressed;
        
        usleep(10000); // 10ms
    }
}

void InputHandler::start() {
    if (running) return;
    
    running = true;
    inputThread = std::thread([this]() { inputLoop(); });
}

void InputHandler::stop() {
    running = false;
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

int InputHandler::keySymToKeycode(Display* dpy, const std::string& keySym) {
    KeySym ks = XStringToKeysym(keySym.c_str());
    if (ks == NoSymbol) return 0;
    return XKeysymToKeycode(dpy, ks);
}

std::string InputHandler::keycodeToKeySym(Display* dpy, int keycode) {
    if (keycode == 0) return "";
    
    KeySym ks = XKeycodeToKeysym(dpy, keycode, 0);
    if (ks == NoSymbol) return "";
    
    return XKeysymToString(ks);
}

void InputHandler::checkInput() {
    static bool toggleWasPressed = false;
    static bool holdWasPressed = false;

    bool togglePressed = isKeyPressed(toggleKeycode);
    bool holdPressed = isKeyPressed(holdKeycode);

    // Toggle mode
    if (togglePressed && !toggleWasPressed && toggleCallback) {
        toggleCallback();
    }

    // Hold mode
    if (holdModeEnabled) {
        if (holdPressed != holdWasPressed && holdCallback) {
            holdCallback(holdPressed);
        }
    }

    toggleWasPressed = togglePressed;
    holdWasPressed = holdPressed;
}
