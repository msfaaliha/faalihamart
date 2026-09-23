# Multi-stage Dockerfile for FaalihaMart (Section 8 Deployment Specification)
# Stage 1: Build stage with full C++20 toolchain and dependencies
FROM debian:bookworm-slim AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    libsqlite3-dev \
    libsodium-dev \
    libssl-dev \
    uuid-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Clone and install Drogon web framework
WORKDIR /tmp
RUN git clone https://github.com/drogonframework/drogon.git && \
    cd drogon && \
    git submodule update --init && \
    mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF .. && \
    make -j$(nproc) && \
    make install && \
    cd /tmp && rm -rf drogon

# Build FaalihaMart application
WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --config Release -j$(nproc)

# Stage 2: Minimal runtime image containing only binary and runtime shared libraries
FROM debian:bookworm-slim AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsqlite3-0 \
    libsodium23 \
    libssl3 \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Create application user for security
RUN useradd -m -u 1001 -s /bin/bash faalihamart

WORKDIR /opt/faalihamart
COPY --from=builder /app/build/faalihamart /opt/faalihamart/bin/faalihamart
COPY --from=builder /app/static /opt/faalihamart/static
COPY --from=builder /app/db /opt/faalihamart/db
COPY --from=builder /app/config.json /opt/faalihamart/config.json

RUN mkdir -p /opt/faalihamart/logs && \
    chown -R faalihamart:faalihamart /opt/faalihamart

USER faalihamart

EXPOSE 8080

HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
  CMD curl -f http://localhost:8080/api/v1/health || exit 1

ENTRYPOINT ["/opt/faalihamart/bin/faalihamart"]
