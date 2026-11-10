@echo off
title FaalihaMart Cloudflare Tunnel Launcher
cd /d "%~dp0"
echo ==========================================================
echo Starting FaalihaMart on Localhost + Cloudflare Tunnel...
echo Local URL: http://localhost:8080/index.html
echo ==========================================================
powershell -ExecutionPolicy Bypass -File "%~dp0start_cloudflare.ps1"
pause
