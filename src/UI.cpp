#include "UI.h"
#include <iostream>
#include <X11/Xlib.h>
#include <X11/keysym.h>

UI::UI(AutoClicker* clicker) : clicker(clicker), listeningForToggleKey(false), listeningForHoldKey(false) {
    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "ClickWise Auto-Clicker");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    
    // Connect key press event
    g_signal_connect(window, "key-press-event", G_CALLBACK(onKeyPress), this);
    
    // Main vertical box
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Status label
    statusLabel = gtk_label_new("Auto-Clicker: Off");
    gtk_box_pack_start(GTK_BOX(vbox), statusLabel, FALSE, FALSE, 0);
    
    // Separator
    gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
    
    // Click interval
    GtkWidget* intervalFrame = gtk_frame_new("Click Interval (ms)");
    gtk_box_pack_start(GTK_BOX(vbox), intervalFrame, FALSE, FALSE, 0);
    
    GtkWidget* intervalBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(intervalFrame), intervalBox);
    
    intervalSpin = gtk_spin_button_new_with_range(1, 5000, 1); // Minimum changed to 1ms
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(intervalSpin), clicker->getClickInterval());
    gtk_box_pack_start(GTK_BOX(intervalBox), intervalSpin, TRUE, TRUE, 0);
    
    // Hold mode
    holdModeCheck = gtk_check_button_new_with_label("Enable hold mode");
    gtk_box_pack_start(GTK_BOX(vbox), holdModeCheck, FALSE, FALSE, 0);
    
    // Key bindings frame
    GtkWidget* keysFrame = gtk_frame_new("Key Bindings");
    gtk_box_pack_start(GTK_BOX(vbox), keysFrame, FALSE, FALSE, 0);
    
    GtkWidget* keysGrid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(keysGrid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(keysGrid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(keysGrid), 5);
    gtk_container_add(GTK_CONTAINER(keysFrame), keysGrid);
    
    // Toggle key
    gtk_grid_attach(GTK_GRID(keysGrid), gtk_label_new("Toggle Key:"), 0, 0, 1, 1);
    
    toggleKeyBtn = gtk_button_new_with_label("Bind");
    gtk_grid_attach(GTK_GRID(keysGrid), toggleKeyBtn, 1, 0, 1, 1);
    
    toggleKeyLabel = gtk_label_new("F6");
    gtk_grid_attach(GTK_GRID(keysGrid), toggleKeyLabel, 2, 0, 1, 1);
    
    // Hold key
    gtk_grid_attach(GTK_GRID(keysGrid), gtk_label_new("Hold Key:"), 0, 1, 1, 1);
    
    holdKeyBtn = gtk_button_new_with_label("Bind");
    gtk_grid_attach(GTK_GRID(keysGrid), holdKeyBtn, 1, 1, 1, 1);
    
    holdKeyLabel = gtk_label_new("Caps_Lock");
    gtk_grid_attach(GTK_GRID(keysGrid), holdKeyLabel, 2, 1, 1, 1);
    
    // Load config
    ConfigManager::Config config;
    configManager.loadConfig(config);
    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(intervalSpin), config.clickInterval);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(holdModeCheck), config.holdModeEnabled);
    gtk_label_set_text(GTK_LABEL(toggleKeyLabel), config.toggleKey.c_str());
    gtk_label_set_text(GTK_LABEL(holdKeyLabel), config.holdKey.c_str());
    
    // Connect signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(intervalSpin, "value-changed", G_CALLBACK(onIntervalChanged), this);
    g_signal_connect(holdModeCheck, "toggled", G_CALLBACK(onHoldModeToggled), this);
    g_signal_connect(toggleKeyBtn, "clicked", G_CALLBACK(onToggleKeyBind), this);
    g_signal_connect(holdKeyBtn, "clicked", G_CALLBACK(onHoldKeyBind), this);
    
    gtk_widget_show_all(window);
    updateStatus();
}

UI::~UI() {
    saveConfig();
}

void UI::run() {
    gtk_main();
}

void UI::saveConfig() {
    ConfigManager::Config config;
    
    config.clickInterval = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(intervalSpin));
    config.holdModeEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(holdModeCheck));
    config.toggleKey = gtk_label_get_text(GTK_LABEL(toggleKeyLabel));
    config.holdKey = gtk_label_get_text(GTK_LABEL(holdKeyLabel));
    
    configManager.saveConfig(config);
    
    // Update the auto-clicker with new settings
    clicker->setClickInterval(config.clickInterval);
    clicker->setHoldMode(config.holdModeEnabled);
    
    // Update key bindings
    Display* display = XOpenDisplay(NULL);
    if (display) {
        int toggleKeycode = XKeysymToKeycode(display, XStringToKeysym(config.toggleKey.c_str()));
        int holdKeycode = XKeysymToKeycode(display, XStringToKeysym(config.holdKey.c_str()));
        
        if (toggleKeycode != NoSymbol) {
            clicker->updateToggleKey(toggleKeycode);
        }
        
        if (holdKeycode != NoSymbol) {
            clicker->updateHoldKey(holdKeycode);
        }
        
        XCloseDisplay(display);
    }
}

void UI::updateStatus() {
    bool clicking = clicker->isClicking();
    gtk_label_set_text(GTK_LABEL(statusLabel), 
                      clicking ? "Auto-Clicker: Active" : "Auto-Clicker: Off");
}

void UI::startKeyListening(bool forToggleKey) {
    listeningForToggleKey = forToggleKey;
    listeningForHoldKey = !forToggleKey;
    
    if (forToggleKey) {
        gtk_button_set_label(GTK_BUTTON(toggleKeyBtn), "Press any key...");
    } else {
        gtk_button_set_label(GTK_BUTTON(holdKeyBtn), "Press any key...");
    }
    
    gtk_widget_set_sensitive(toggleKeyBtn, FALSE);
    gtk_widget_set_sensitive(holdKeyBtn, FALSE);
}

void UI::stopKeyListening() {
    listeningForToggleKey = false;
    listeningForHoldKey = false;
    
    gtk_button_set_label(GTK_BUTTON(toggleKeyBtn), "Bind");
    gtk_button_set_label(GTK_BUTTON(holdKeyBtn), "Bind");
    
    gtk_widget_set_sensitive(toggleKeyBtn, TRUE);
    gtk_widget_set_sensitive(holdKeyBtn, TRUE);
}

void UI::processKeyPress(int keycode) {
    if (!listeningForToggleKey && !listeningForHoldKey) return;
    
    Display* display = XOpenDisplay(NULL);
    if (!display) return;
    
    KeySym keysym = XKeycodeToKeysym(display, keycode, 0);
    std::string keySymStr = XKeysymToString(keysym);
    
    XCloseDisplay(display);
    
    if (keySymStr.empty()) return;
    
    if (listeningForToggleKey) {
        gtk_label_set_text(GTK_LABEL(toggleKeyLabel), keySymStr.c_str());
    } else if (listeningForHoldKey) {
        gtk_label_set_text(GTK_LABEL(holdKeyLabel), keySymStr.c_str());
    }
    
    stopKeyListening();
    saveConfig(); // Save config and update key bindings immediately
}

// Static callback functions
void UI::onIntervalChanged(GtkWidget* widget, gpointer data) {
    UI* ui = static_cast<UI*>(data);
    int interval = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
    ui->clicker->setClickInterval(interval);
    ui->saveConfig();
}

void UI::onHoldModeToggled(GtkWidget* widget, gpointer data) {
    UI* ui = static_cast<UI*>(data);
    bool enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    ui->clicker->setHoldMode(enabled);
    ui->saveConfig();
}

void UI::onToggleKeyBind(GtkWidget* widget, gpointer data) {
    UI* ui = static_cast<UI*>(data);
    ui->startKeyListening(true);
}

void UI::onHoldKeyBind(GtkWidget* widget, gpointer data) {
    UI* ui = static_cast<UI*>(data);
    ui->startKeyListening(false);
}

gboolean UI::onKeyPress(GtkWidget* widget, GdkEventKey* event, gpointer data) {
    UI* ui = static_cast<UI*>(data);
    
    if (ui->listeningForToggleKey || ui->listeningForHoldKey) {
        ui->processKeyPress(event->hardware_keycode);
        return TRUE; // Stop event propagation
    }
    
    return FALSE; // Continue event propagation
}