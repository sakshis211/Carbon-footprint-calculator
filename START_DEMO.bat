@echo off
color 0A
echo ===================================================
echo     STARTING ENTERPRISE ECO-CALC ENGINE
echo ===================================================
echo.

echo [1/3] Compiling C++ Backend to ensure latest version...
g++ backend.cpp -o backend.exe
echo Done.
echo.

echo [2/3] Launching User Interface in default browser...
start http://127.0.0.1:5000
echo.

echo [3/3] Booting Python Flask Server...
echo (Keep this window open during your presentation!)
echo.
python app.py