# FaalihaMart Localhost + Cloudflare Tunnel Launcher
$port = 8080
$previewScript = Join-Path $PSScriptRoot "start_preview.ps1"

# Start local preview server in background if port 8080 is not already listening
$listening = Get-NetTCPConnection -LocalPort $port -State Listen -ErrorAction SilentlyContinue
if (-not $listening) {
    Write-Host "Starting local server on http://localhost:$port ..." -ForegroundColor Cyan
    Start-Process powershell -ArgumentList "-NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File `"$previewScript`""
    Start-Sleep -Seconds 2
}

$localCloudflaredDir = Join-Path $env:LOCALAPPDATA "cloudflared"
$localCloudflaredExe = Join-Path $localCloudflaredDir "cloudflared.exe"

# Locate cloudflared executable
$cloudflaredCmd = Get-Command cloudflared -ErrorAction SilentlyContinue
$cloudflaredExe = if ($cloudflaredCmd) {
    $cloudflaredCmd.Source
} else {
    @(
        $localCloudflaredExe,
        "$env:ProgramFiles\cloudflared\cloudflared.exe",
        "${env:ProgramFiles(x86)}\cloudflared\cloudflared.exe",
        (Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages" -Filter "cloudflared.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName)
    ) | Where-Object { $_ -and (Test-Path $_) } | Select-Object -First 1
}

if (-not $cloudflaredExe) {
    Write-Host "cloudflared not found. Installing via winget (--source winget)..." -ForegroundColor Yellow
    try {
        winget install --id Cloudflare.cloudflared --source winget -e --accept-package-agreements --accept-source-agreements
    } catch {}
    $cloudflaredExe = @(
        "$env:ProgramFiles\cloudflared\cloudflared.exe",
        "${env:ProgramFiles(x86)}\cloudflared\cloudflared.exe",
        (Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages" -Filter "cloudflared.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName)
    ) | Where-Object { $_ -and (Test-Path $_) } | Select-Object -First 1
}

if (-not $cloudflaredExe) {
    Write-Host "Downloading official cloudflared binary for Windows..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Force -Path $localCloudflaredDir | Out-Null
    Invoke-WebRequest -Uri "https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-windows-amd64.exe" -OutFile $localCloudflaredExe -UseBasicParsing
    if (Test-Path $localCloudflaredExe) {
        $cloudflaredExe = $localCloudflaredExe
    }
}

if (-not $cloudflaredExe) {
    Write-Host "Failed to locate cloudflared.exe. Please restart your terminal or install Cloudflare.cloudflared." -ForegroundColor Red
    exit 1
}

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "  Launching FaalihaMart Localhost on Cloudflare Tunnel" -ForegroundColor Green
Write-Host "  Local URL: http://localhost:$port/index.html" -ForegroundColor Yellow
Write-Host "==========================================================" -ForegroundColor Cyan

& $cloudflaredExe tunnel --no-autoupdate --protocol http2 --url "http://localhost:$port" --http-host-header "localhost:$port"
