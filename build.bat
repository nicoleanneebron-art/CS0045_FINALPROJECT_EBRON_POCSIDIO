@echo off
setlocal

rem Keep the MSYS2 UCRT64 runtime DLLs visible to g++. This includes the
rem compiler dependencies such as libgmp, libmpc, libisl, and libmpfr.
if exist "C:\msys64\ucrt64\bin" set "PATH=C:\msys64\ucrt64\bin;%PATH%"

rem This project keeps runtime DLLs beside the executable. A compiler and
rem the matching FreeGLUT/GLEW development files are required to rebuild it.
set "MINGW=%~dp0mingw32"
set "COMPILER="

if exist "C:\msys64\ucrt64\bin\g++.exe" (
  set "COMPILER=C:\msys64\ucrt64\bin\g++.exe"
  set "MINGW=C:\msys64\ucrt64"
)

if not defined COMPILER (
  where g++.exe >nul 2>&1
  if not errorlevel 1 set "COMPILER=g++"
)

if not defined COMPILER if exist "%~dp0mingw32\bin\g++.exe" (
  set "COMPILER=%~dp0mingw32\bin\g++.exe"
  set "MINGW=%~dp0mingw32"
)

if not defined COMPILER if exist "%~dp0..\mingw32\bin\g++.exe" (
  set "COMPILER=%~dp0..\mingw32\bin\g++.exe"
  set "MINGW=%~dp0..\mingw32"
)

if not defined COMPILER (
  echo ERROR: g++.exe was not found.
  echo Install a 64-bit MinGW-w64 toolchain and add its bin folder to PATH.
  exit /b 1
)

if not exist "%MINGW%\include\GL\glew.h" (
  echo ERROR: GLEW development headers were not found at "%MINGW%\include\GL\glew.h".
  echo Install the FreeGLUT/GLEW development packages and set MINGW accordingly.
  exit /b 1
)

if not exist "%MINGW%\lib\libglew32.a" (
  echo ERROR: GLEW import library was not found at "%MINGW%\lib\libglew32.a".
  echo Install the FreeGLUT/GLEW development packages and set MINGW accordingly.
  exit /b 1
)

"%COMPILER%" -O2 src\main.cpp src\Application.cpp src\RenderingModule.cpp src\SceneState.cpp src\GridModule.cpp src\CameraModule.cpp src\WaterModule.cpp src\FloatingIslandsModule.cpp src\TerrainModule.cpp src\SkyModule.cpp src\ButterflyModule.cpp src\CrystalModule.cpp ^
  -I"%MINGW%\include" -I"%~dp0include" -L"%MINGW%\lib" ^
  -lglew32 -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 ^
  -o DadahRangePrototype.exe

if errorlevel 1 exit /b %errorlevel%
for %%F in (glew32.dll libfreeglut.dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
  if exist "%MINGW%\bin\%%F" copy /Y "%MINGW%\bin\%%F" "%~dp0%%F" >nul
)
echo Built DadahRangePrototype.exe successfully.
