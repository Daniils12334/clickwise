#include "ApplicationFilter.h"
#include <X11/Xutil.h>
#include <algorithm>

ApplicationFilter::ApplicationFilter(Display* dpy) : display(dpy) {}

void ApplicationFilter::setExcludedApps(const std::vector<std::string>& apps) {
    excludedApps = apps;
}

std::string ApplicationFilter::getActiveWindowClass() {
    Window focused;
    int revert;
    XGetInputFocus(display, &focused, &revert);
    
    XClassHint classHint;
    if (XGetClassHint(display, focused, &classHint)) {
        std::string cls(classHint.res_class ? classHint.res_class : "");
        XFree(classHint.res_name);
        XFree(classHint.res_class);
        return cls;
    }
    return "";
}

bool ApplicationFilter::shouldClick() {
    if (excludedApps.empty()) return true;
    
    std::string activeClass = getActiveWindowClass();
    if (activeClass.empty()) return true;
    
    return std::find(excludedApps.begin(), excludedApps.end(), activeClass) == excludedApps.end();
}