@echo off
title FaalihaMart Web Preview Server
cd /d "%~dp0"
echo ==========================================================
echo Starting FaalihaMart Web Application Preview...
echo URL: http://localhost:8080/index.html
echo ==========================================================
powershell -ExecutionPolicy Bypass -File "%~dp0start_preview.ps1"
pause
