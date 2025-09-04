#ifndef APPLICATIONFILTER_H
#define APPLICATIONFILTER_H

#include <string>
#include <vector>
#include <X11/Xlib.h>

class ApplicationFilter {
private:
    std::vector<std::string> excludedApps;
    Display* display;
    
    std::string getActiveWindowClass();
    
public:
    ApplicationFilter(Display* dpy);
    void setExcludedApps(const std::vector<std::string>& apps);
    bool shouldClick();
};

#endif