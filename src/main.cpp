#include "AutoClicker.h"
#include "UI.h"
#include <gtk/gtk.h>
#include <thread>

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    
    AutoClicker clicker;
    UI ui(&clicker);
    
    // Start auto-clicker in background thread
    std::thread clickerThread([&clicker]() {
        clicker.start();
    });
    
    // Run UI in main thread
    ui.run();
    
    clicker.stop();
    if (clickerThread.joinable()) {
        clickerThread.join();
    }
    
    return 0;
}