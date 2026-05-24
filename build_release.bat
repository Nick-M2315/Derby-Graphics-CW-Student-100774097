@echo off
REM Build script for Release configuration
REM This creates a bin folder with the Release executable

setlocal enabledelayedexpansion

echo.
echo ================================================================================
echo                    Building Release Configuration
echo ================================================================================
echo.

REM Check if build_release directory exists
if not exist build_release (
    echo Creating build_release directory...
    mkdir build_release
    echo.
)

cd build_release

echo Configuring CMake for Release build...
echo Command: cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
echo.
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    echo Please ensure:
    echo   - CMake 3.26.0 or higher is installed
    echo   - Ninja generator is installed
    echo   - Visual Studio build tools are available
    echo.
    cd ..
    pause
    exit /b 1
)

echo.
echo Building Release executable...
echo Command: cmake --build . --config Release
echo.
cmake --build . --config Release

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    echo Check the output above for details.
    echo.
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ================================================================================
echo                          BUILD COMPLETE!
echo ================================================================================
echo.
echo Executable location: bin\ray05.exe
echo.
echo Next steps:
echo   1. Run: bin\ray05.exe          (starts rendering)
echo   2. Run: bin\run.bat             (opens PPM viewer in browser)
echo   3. Find output in: bin\out\Render Outputs\
echo.
pause
