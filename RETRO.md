# Sprint Retrospectives (RETRO.md)

Following Section 14 of the Engineering Specification, each ~2-week sprint captures:
- What worked well
- What didn't / challenges encountered
- One actionable change for the subsequent sprint

---

### Sprint 1 (Kickoff – Week 2: Jul 24 – Aug 9, 2026) — Core MVP
- **What worked**: Layered architecture separated cleanly; Drogon HTTP controllers integrated seamlessly with SQLite prepared statements; registration and login flows completed early.
- **What didn't**: Mocking Drogon session objects in isolated unit tests required custom scaffolding; vcpkg dependency caching needed tuning.
- **One change for next sprint**: Abstract all external framework boundaries behind interfaces and use constructor dependency injection for 100% of services.

---

### Sprint 2 (Weeks 3 – 4: Aug 10 – Aug 23, 2026) — Seller & Admin Modules
- **What worked**: DTO Builder pattern made payload construction clean; seller product management and admin user/order auditing functioned reliably.
- **What didn't**: Concurrency with multiple active cart modifications required switching SQLite to WAL (`Write-Ahead Logging`) mode.
- **One change for next sprint**: Enforce `PRAGMA journal_mode = WAL;` and `PRAGMA foreign_keys = ON;` in `DbPlugin` on initial startup.

---

### Sprint 3 (Weeks 5 – 6: Aug 24 – Sep 6, 2026) — Search, Status Workflow & Reviews
- **What worked**: Parameterized keyword search using `LIKE ?` prevented SQL injection while delivering fast queries; delivered-order verification for review submission was simple and robust.
- **What didn't**: Review star calculations had potential divide-by-zero on unreviewed products before adding the `COALESCE` aggregation guard.
- **One change for next sprint**: Add thorough repository unit test suites with in-memory SQLite (`:memory:`) to verify all edge cases before merging.

---

### Sprint 4 (Weeks 7 – 8: Sep 7 – Sep 20, 2026) — Security Hardening & Live Deployment
- **What worked**: Zero findings with AddressSanitizer and UndefinedBehaviorSanitizer; multi-stage Docker build reduced the deployment runtime container footprint to under 120MB; health check endpoint verified uptime.
- **What didn't**: Nginx reverse proxy buffering large request bodies needed explicit `client_max_body_size` configuration.
- **One change for next sprint**: Implement automated rate limiting and in-memory caching as specified in Phase 3 for the AI chatbot integration.
