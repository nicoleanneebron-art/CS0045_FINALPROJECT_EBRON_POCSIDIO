# Dadah Range — Floating Island Exploration

Dadah Range is a CS0045 Computer Graphics final project built with C++, OpenGL, GLEW, and FreeGLUT. Players explore a dreamy floating-island environment as a butterfly, featuring a crystal sanctuary, central fountain, lake, celestial gateway, animated scenery, and fixed-function OpenGL effects.

## Build and run

Run `build.bat` from this folder. The script builds `DadahRangePrototype.exe` using the existing MinGW, FreeGLUT, GLEW, and Windows multimedia dependencies.

The background music file must remain at `assets/audio/music.wav`. The executable resolves this file relative to its own folder, so it can also be launched from File Explorer.

## Controls

- `W`, `A`, `S`, `D` or arrow keys: butterfly flight movement
- `Shift`: temporary flight-speed boost
- Mouse: continuous mouse-look
- `P`: switch between third-person and first-person views
- `C` (hold): third-person Free-Look
- Mouse wheel: scale the butterfly
- `G`: smooth return to low-hover height
- `F`: toggle fullscreen and restore the previous window size/position
- Left-click: toggle floating-island orbiting
- Right-click: rotate the magical crystal by 30 degrees
- `M`: mute or restart background music
- `Esc`: exit

## Main technical features

- Butterfly wings use Module 4-style client-memory Vertex Arrays with `GLfloat` position/color arrays, `glVertexPointer()`, `glColorPointer()`, and `glDrawArrays()`.
- Terrain, water, rocks, props, grass, and floating islands use VBO-backed geometry.
- Floating islands hover automatically using elapsed-time animation; selected islands can orbit after left-click.
- The crystal rotates independently on right-click, while the sanctuary remains fixed.
- Mouse-wheel scaling uses `glScalef()` inside the butterfly's isolated matrix transform.
- The fountain uses vertically animated water curtains, highlights, ripples, and splash effects.
- Fixed-function lighting, fog, blending, matrix stacks, primitive types, and vertex colors are used throughout the environment.
- Background music uses the Windows MCI `waveaudio` interface and a lightweight status check to restart playback after the WAV ends.

## Source modules

- `Application.cpp`: startup, GLUT callback registration, VBO generation, and scene display flow
- `CameraModule.cpp/.h`: movement, mouse-look, views, controls, collision handling, fullscreen, and input safety
- `ButterflyModule.cpp/.h`: client-memory Vertex Array butterfly geometry and animation
- `CrystalModule.cpp/.h`: rotating crystal and sanctuary rendering
- `FloatingIslandsModule.cpp/.h`: island geometry, timed hovering/orbiting, fountain, and island decorations
- `TerrainModule.cpp/.h`: terrain VBO rendering, scenic landmarks, vegetation, particles, and the Celestial Gateway
- `WaterModule.cpp/.h`: water VBO rendering, lake plants, ripples, and sparkles
- `SkyModule.cpp/.h`: golden-hour sky and cloud rendering
- `AudioModule.cpp/.h`: Windows MCI background music playback and mute control
- `SceneConfig.h` / `SceneState.h`: shared configuration and runtime state

## Notes

The application starts windowed. Press `F` to enter fullscreen mode. If music diagnostics are needed, `audio_debug.log` is created beside the executable at runtime.
