# Technical Debt Register

> Issues identified during Phase 1 code reviews that are non-blocking for current directives but **must** be addressed in Phase 4 (Quality).

| ID | Description | Source | Priority | Target Phase |
|---|---|---|---|---|
| TD-001 | `HomeMembershipGuard` fail-open when `homeId` missing from params — silently passes through instead of rejecting | Directive 1.1 review | Medium | Phase 4 |
| TD-002 | Membership check queries DB on every request — no cache. Under 1000 req/s load this creates 1000 membership queries/s | Directive 1.1 review | High | Phase 3 (Redis) |
| TD-003 | `HomeMembershipGuard` does not store `role` from `HomeMember` on the request — limits future RBAC | Directive 1.1 review | Low | Directive 2.4 |
| TD-004 | `env.validation.ts` accepts whitespace-only values (e.g. `JWT_SECRET=" "` passes validation) | Directive 1.2 review | Medium | Phase 4 |
| TD-005 | No format validation on env vars — `DATABASE_URL` could be any string, not `postgresql://` URL | Directive 1.2 review | Medium | Phase 4 (Joi) |
| TD-006 | `UsageController` and `HomeUsageController` define duplicate routes for `GET /homes/:homeId/usage` | Code audit | Low | Phase 4 |
