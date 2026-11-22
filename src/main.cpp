#include "Application.h"

#if defined(_WIN32)
// Ask NVIDIA Optimus systems to use the discrete GPU for this executable.
// These exports are read by the graphics driver before the OpenGL context is created.
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int main(int argc, char** argv) {
    return application::run(argc, argv);
}
