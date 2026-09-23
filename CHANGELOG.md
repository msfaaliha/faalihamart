# Changelog

All notable changes to the FaalihaMart marketplace application will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), adhering to [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-09-21 (Full Build Demo & Live Deployment Checkpoint)
- Finalized production deployment configuration with multi-stage Docker build, systemd service, and Nginx reverse proxy.
- Verified live URL deployment, health check endpoint (`GET /api/v1/health`), and smoke tests.
- Completed security hardening: 100% parameterized SQL queries, Argon2id password hashing, and zero compiler sanitizer warnings.

## [0.4.0] - 2026-09-06
- Implemented product reviews and star ratings (F8) with purchase verification on DELIVERED orders.
- Added comprehensive field-level input validation across all service methods via `ValidationUtil`.

## [0.3.0] - 2026-08-30
- Implemented catalog search and category filtering (F3).
- Added multi-step order status workflow: `PENDING` -> `CONFIRMED` -> `SHIPPED` -> `DELIVERED` (O2).

## [0.2.0] - 2026-08-23
- Completed seller dashboard for product listing CRUD (F2) and sales revenue analytics (O3).
- Created admin console for user management and listing moderation (F7).

## [0.1.0] - 2026-08-10 (MVP Review)
- Initialized core marketplace flow: user authentication (F1), product browsing, shopping cart (F4), and checkout via mock payment confirmation (F5).
- Implemented layered C++ architecture: Controllers, Services, Repositories, DTOs, and SQLite database migration runner.
