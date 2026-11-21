@echo off
setlocal
set "ROOT=%~dp0..\"
set "MINGW=%ROOT%mingw32"
set "COMPILER=%ROOT%Main-Folder\mingw32"

"%COMPILER%\bin\g++.exe" src\main.cpp src\Application.cpp src\RenderingModule.cpp src\SceneState.cpp src\GridModule.cpp src\CameraModule.cpp src\WaterModule.cpp src\FloatingIslandsModule.cpp src\TerrainModule.cpp src\SkyModule.cpp ^
  -I"%MINGW%\include" -I"%~dp0include" -L"%MINGW%\lib" ^
  -lglew32 -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 ^
  -o DadahRangePrototype.exe

if errorlevel 1 exit /b %errorlevel%
copy /Y "%MINGW%\bin\freeglut.dll" . >nul
copy /Y "%MINGW%\bin\glew32.dll" . >nul
echo Built DadahRangePrototype.exe successfully.
