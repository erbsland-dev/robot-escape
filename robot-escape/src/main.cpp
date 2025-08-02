
#include "Application.hpp"

int main(int argc, char **argv) {
    Application app;
    app.parseArgs(argc, argv);
    app.readConfiguration();
    app.run();
}


