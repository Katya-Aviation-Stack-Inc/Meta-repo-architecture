@echo off
REM Revolutionary Aviation Systems build script for Windows
REM This script builds all revolutionary aviation technologies

echo ========================================
echo Building Revolutionary Aviation Systems
echo ========================================

REM Create build directory
echo Creating build directory...
mkdir build 2>nul
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build the revolutionary systems
echo Building the revolutionary systems...
cmake --build . --config Release

REM Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo ✓ Revolutionary aviation systems built successfully!
    
    REM Display build artifacts
    echo.
    echo Build Artifacts:
    if exist airmesh_demo.exe (
        echo   - airmesh_demo.exe (AAMN demonstration)
    ) else (
        echo   - airmesh_demo.exe (not found)
    )
    if exist revolutionary_demo.exe (
        echo   - revolutionary_demo.exe (full technology demonstration)
    ) else (
        echo   - revolutionary_demo.exe (not found)
    )
    
    echo.
    echo To run the AAMN demonstration:
    echo   cd build ^&^& airmesh_demo.exe
    echo.
    echo To run the full revolutionary demonstration:
    echo   cd build ^&^& revolutionary_demo.exe
    
    echo.
    echo ========================================
    echo Revolutionary Build Complete!
    echo ========================================
) else (
    echo ✗ Failed to build revolutionary aviation systems!
    exit /b 1
)