# Dadah Range Prototype

First playable blockout for the floating-island butterfly exploration concept.

## Build

Run `build.bat` from this folder. The script uses the repository's existing MinGW, FreeGLUT, and GLEW files.

Project layout:

- `src/`: C++ implementation files
- `include/`: shared headers and configuration
- Project root: build script, executable, and runtime DLLs

## Source modules

- `main.cpp`: program entry point
- `Application.cpp`: window initialization, VBO generation, scene orchestration, and display loop
- `CameraModule.cpp/.h`: camera movement, mouse look, zoom, and ground-level mode
- `TerrainModule.cpp/.h`: land, grass, mounds, and terrain rendering
- `WaterModule.cpp/.h`: animated water surface and water rendering
- `FloatingIslandsModule.cpp/.h`: island placement, rings, and hovering animation
- `GridModule.cpp/.h`: floor-grid rendering
- `RenderingModule.cpp/.h`: shared materials and lighting
- `SceneConfig.h` / `SceneState.h`: shared scene configuration and runtime state

## Controls

- `W`, `A`, `S`, `D`: move the inspection camera
- Mouse: look around while inside the window
- Mouse wheel: zoom in and out
- `G`: toggle ground-level inspection view
- `Esc`: exit

## Current prototype scope

- Grid-aligned bumpy land and basin-like lakes
- Textured water surfaces with linear filtering
- Central mound range
- Layered, asymmetrical floating-island formations
- High scattered and skyward island layers
- VBO-backed grid, land, water, mound, and island geometry
- Lit 3D scene using the Module 8 fixed-function approach
- Mouse-look and perspective camera navigation based on Module 7 patterns
