#!/usr/bin/env bash
set -euo pipefail

echo "=========================================================="
echo "  FaalihaMart Automated Deployment Script                 "
echo "  Milestone: September 21 (2nd Review)                    "
echo "=========================================================="

APP_DIR="/opt/faalihamart"
mkdir -p "${APP_DIR}/bin" "${APP_DIR}/data" "${APP_DIR}/logs"

echo "[1/4] Building release binary with CMake..."
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release -j"$(nproc)"
cd ..

echo "[2/4] Copying binary and assets to ${APP_DIR}..."
cp build/faalihamart "${APP_DIR}/bin/"
cp -r static "${APP_DIR}/"
cp -r db "${APP_DIR}/"
cp config.json "${APP_DIR}/"

echo "[3/4] Installing systemd service..."
cp deploy/faalihamart.service /etc/systemd/system/
systemctl daemon-reload
systemctl enable faalihamart
systemctl restart faalihamart

echo "[4/4] Verifying health check..."
sleep 2
curl -f http://localhost:8080/api/v1/health || {
    echo "Health check failed!"
    exit 1
}

echo "Deployment successful! FaalihaMart is running on port 8080."
