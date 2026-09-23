# FaalihaMart Local Preview Server (Robust Daemon Edition)
$port = 8080
$prefix = "http://localhost:$port/"
$staticDir = Join-Path $PSScriptRoot "static"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "  FaalihaMart Web Preview Server (Port $port)" -ForegroundColor Green
Write-Host "  Serving: $staticDir" -ForegroundColor Yellow
Write-Host "  URL: $prefix" -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add($prefix)

try {
    $listener.Start()
    Write-Host "Server listening at $prefix" -ForegroundColor Green
} catch {
    Write-Host "Failed to bind to $prefix : $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

while ($listener.IsListening) {
    try {
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response

        # Add CORS headers
        $response.AddHeader("Access-Control-Allow-Origin", "*")
        $response.AddHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS")
        $response.AddHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")

        if ($request.HttpMethod -eq "OPTIONS") {
            $response.StatusCode = 204
            $response.Close()
            continue
        }

        $urlPath = $request.Url.LocalPath.TrimStart('/')
        if ([string]::IsNullOrWhiteSpace($urlPath)) {
            $urlPath = "index.html"
        }

        # Health endpoint
        if ($urlPath -eq "api/v1/health") {
            $json = '{"success":true,"data":{"status":"UP","db":"UP"},"error":null}'
            $bytes = [System.Text.Encoding]::UTF8.GetBytes($json)
            $response.ContentType = "application/json"
            $response.ContentLength64 = $bytes.Length
            $response.StatusCode = 200
            if ($request.HttpMethod -ne "HEAD") {
                $response.OutputStream.Write($bytes, 0, $bytes.Length)
            }
            $response.OutputStream.Close()
            continue
        }

        $filePath = Join-Path $staticDir $urlPath
        if (Test-Path $filePath -PathType Leaf) {
            $ext = [System.IO.Path]::GetExtension($filePath).ToLower()
            $contentType = switch ($ext) {
                ".html" { "text/html; charset=utf-8" }
                ".css"  { "text/css; charset=utf-8" }
                ".js"   { "application/javascript; charset=utf-8" }
                ".json" { "application/json" }
                ".png"  { "image/png" }
                ".jpg"  { "image/jpeg" }
                ".svg"  { "image/svg+xml" }
                default { "application/octet-stream" }
            }

            $bytes = [System.IO.File]::ReadAllBytes($filePath)
            $response.ContentType = $contentType
            $response.ContentLength64 = $bytes.Length
            $response.StatusCode = 200
            if ($request.HttpMethod -ne "HEAD") {
                $response.OutputStream.Write($bytes, 0, $bytes.Length)
            }
            $response.OutputStream.Close()
        } else {
            $response.StatusCode = 404
            $err = [System.Text.Encoding]::UTF8.GetBytes("File Not Found")
            $response.ContentLength64 = $err.Length
            if ($request.HttpMethod -ne "HEAD") {
                $response.OutputStream.Write($err, 0, $err.Length)
            }
            $response.OutputStream.Close()
        }
    } catch {
        # Catch individual request exceptions without crashing listener
        Write-Host "Request warning: $($_.Exception.Message)" -ForegroundColor DarkGray
    }
}
