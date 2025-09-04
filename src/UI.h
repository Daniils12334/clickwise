#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include "AutoClicker.h"

class UI {
private:
    GtkWidget* window;
    GtkWidget* intervalSpin;
    GtkWidget* holdModeCheck;
    GtkWidget* toggleKeyBtn;
    GtkWidget* holdKeyBtn;
    GtkWidget* toggleKeyLabel;
    GtkWidget* holdKeyLabel;
    GtkWidget* statusLabel;
    
    AutoClicker* clicker;
    ConfigManager configManager;
    
    bool listeningForToggleKey;
    bool listeningForHoldKey;
    
    static void onIntervalChanged(GtkWidget* widget, gpointer data);
    static void onHoldModeToggled(GtkWidget* widget, gpointer data);
    static void onToggleKeyBind(GtkWidget* widget, gpointer data);
    static void onHoldKeyBind(GtkWidget* widget, gpointer data);
    static gboolean onKeyPress(GtkWidget* widget, GdkEventKey* event, gpointer data);
    
    void updateStatus();
    void startKeyListening(bool forToggleKey);
    void stopKeyListening();
    void processKeyPress(int keycode);
    
public:
    UI(AutoClicker* clicker);
    ~UI();
    
    void run();
    void saveConfig();
};

#endif