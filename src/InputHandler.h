#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <X11/Xlib.h>
#include <string>
#include <functional>
#include <atomic>
#include <thread>

class InputHandler {
private:
    Display* display;
    std::function<void()> toggleCallback;
    std::function<void(bool)> holdCallback;
    
    std::atomic<int> toggleKeycode;
    std::atomic<int> holdKeycode;
    std::atomic<bool> holdModeEnabled;
    std::atomic<bool> running;
    
    std::thread inputThread;
    
    bool isKeyPressed(int keycode);
    void inputLoop();
    
public:
    InputHandler(Display* dpy);
    ~InputHandler();
    
    void setToggleKey(int keycode);
    void setHoldKey(int keycode);
    void setHoldMode(bool enabled);
    
    void checkInput();
    
    void setToggleCallback(std::function<void()> callback);
    void setHoldCallback(std::function<void(bool)> callback);
    
    void start();
    void stop();
    
    int getCurrentToggleKeycode() const { return toggleKeycode.load(); }
    int getCurrentHoldKeycode() const { return holdKeycode.load(); }
    
    static int keySymToKeycode(Display* dpy, const std::string& keySym);
    static std::string keycodeToKeySym(Display* dpy, int keycode);
};

#endif