# Technical Debt Register

> Issues identified during Phase 1 code reviews that are non-blocking for current directives but **must** be addressed in Phase 4 (Quality).

| ID | Description | Source | Priority | Target Phase |
|---|---|---|---|---|---|
| TD-001 | `HomeMembershipGuard` fail-open when `homeId` missing from params — silently passes through instead of rejecting | Directive 1.1 review | Medium | Phase 4 |
| TD-002 | Membership check queries DB on every request — no cache. Under 1000 req/s load this creates 1000 membership queries/s | Directive 1.1 review | High | Phase 3 (Redis) |
| TD-003 | `HomeMembershipGuard` does not store `role` from `HomeMember` on the request — limits future RBAC | Directive 1.1 review | Low | Directive 2.4 |
| TD-004 | `env.validation.ts` accepts whitespace-only values (e.g. `JWT_SECRET=" "` passes validation) | Directive 1.2 review | Medium | Phase 4 |
| TD-005 | No format validation on env vars — `DATABASE_URL` could be any string, not `postgresql://` URL | Directive 1.2 review | Medium | Phase 4 (Joi) |
| TD-006 | `UsageController` and `HomeUsageController` define duplicate routes for `GET /homes/:homeId/usage` | Code audit | Low | Phase 4 |
| TD-007 | `PAIRING_SECRET` hardcoded in `credential_manager.cpp:12` — same value across all devices, extractable from firmware binary | Phase 1 closure review | High | Phase 2 (Directive 2.2) |
| TD-008 | `deriveKey()` (HKDF-SHA256) called on every encrypt/decrypt — adds 10-20ms latency per operation | Phase 1 closure review | Low | Phase 3 or 4 |
| TD-009 | `salt[SALT_LENGTH - 1] = 0` reduces salt entropy from 128 to 120 bits unnecessarily | Phase 1 closure review | Low | Phase 4 |
| TD-010 | Decrypted plaintext (`std::vector<uint8_t> decrypted`) not explicitly wiped after use — compiler may optimize away memset | Phase 1 closure review | Medium | Phase 4 |
