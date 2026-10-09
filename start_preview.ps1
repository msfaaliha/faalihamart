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

        # AI Chatbot endpoint (Section 9, 15)
        if ($urlPath -eq "api/v1/chat" -or $urlPath -eq "api/chat") {
            $userMsg = ""
            if ($request.HasEntityBody) {
                $reader = New-Object System.IO.StreamReader($request.InputStream, $request.ContentEncoding)
                $bodyStr = $reader.ReadToEnd()
                try {
                    $bodyObj = ConvertFrom-Json $bodyStr
                    $userMsg = $bodyObj.message
                } catch {}
            }

            $q = if ($userMsg) { $userMsg.ToLower() } else { "" }
            $reply = "Hello! Welcome to FaalihaMart. I can assist with products in Electronics, Apparel, and Home & Living, order status tracking (PENDING -> CONFIRMED -> SHIPPED -> DELIVERED), returns, mock payment, or seller registration. How may I help you?"
            
            if ($q -match "headphone|audio|earphone|noise-cancelling") {
                $reply = "We have the 'Noise-Cancelling Wireless Headphones' by Apex Tech Gear for ₹149.99 (40-hour battery life, spatial audio) in our Electronics category."
            } elseif ($q -match "keyboard|rgb|switch") {
                $reply = "The 'Mechanical Gaming Keyboard RGB' is available for ₹89.99 featuring custom mechanical switches and per-key RGB backlighting."
            } elseif ($q -match "monitor|screen|4k") {
                $reply = "Our 'Ultra-Slim 4K USB-C Monitor 27`"' is ₹299.99 with IPS panel, 99% sRGB color gamut, and 65W power delivery."
            } elseif ($q -match "hoodie|cotton|jacket") {
                $reply = "The 'Classic Organic Cotton Hoodie' is ₹49.99 tailored from 100% sustainable organic cotton in the Apparel category."
            } elseif ($q -match "backpack|bag|laptop") {
                $reply = "The 'Waterproof Commuter Backpack' is ₹74.99 with weatherproof construction and a 16-inch laptop compartment."
            } elseif ($q -match "candle|tumbler|home") {
                $reply = "In Home & Living, check out our Soy Wax Candle Set (₹24.99) and Insulated Stainless Steel Tumbler (₹19.99)."
            } elseif ($q -match "track|order|status|workflow|shipped|delivered") {
                $reply = "FaalihaMart orders follow a 4-step workflow: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED. Check the 'My Orders' tab to inspect details!"
            } elseif ($q -match "return|refund|exchange") {
                $reply = "FaalihaMart offers a 7-day hassle-free return window on delivered orders. You can also submit reviews and ratings once delivered."
            } elseif ($q -match "payment|pay|checkout|mock") {
                $reply = "FaalihaMart checkout uses a simulated mock payment confirmation step (Mock Credit Card or Mock UPI). No real card is billed."
            } elseif ($q -match "sell|seller|listing") {
                $reply = "To list products, switch to Seller role in the top header. You'll gain access to the Seller Dashboard for listing CRUD and revenue analytics."
            } elseif ($q -match "review|star|rating") {
                $reply = "Buyers who have purchased a product can submit a 1 to 5 star rating and comment after the order reaches DELIVERED status."
            }

            $respData = @{
                success = $true
                data = @{
                    reply = $reply
                    provider = "mock-preview"
                    cached = $false
                }
                error = $null
            }
            $json = ConvertTo-Json $respData -Depth 4
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
