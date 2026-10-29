@echo off
cd /d "%~dp0backend"
if not exist build\SearchEngine.exe (
  echo Building backend...
  cmake -S . -B build -G Ninja
  cmake --build build
)
echo Starting backend...
build\SearchEngine.exe --server
