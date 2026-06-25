# Developer Guidelines

## Development Environment

### Required Tools

| Tool | Version | Purpose |
|---|---|---|
| Node.js | 18+ | NestJS backend |
| pnpm | 8+ | Package manager |
| Docker | 24+ | Local PostgreSQL, Redis, EMQX |
| Flutter | 3.16+ | Mobile app |
| Arduino IDE / PlatformIO | Latest | ESP32 firmware |
| Git | 2+ | Version control |

### Quick Start

```bash
# Clone
git clone <repo-url>
cd emo-home-security

# Backend
cd cloud
cp ../.env.example .env
docker compose up -d        # Start DB + Redis + MQTT
pnpm install
pnpm run start:dev

# Flutter App
cd ../flutter_app
flutter pub get
flutter run

# ESP32 Firmware
cd ../esp32_firmware
# Open in PlatformIO or Arduino IDE
# Configure WiFi credentials & MQTT broker URL
# Flash to device
```

---

## Repository Structure

```
emo-home-security/
├── cloud/                  # NestJS backend
│   ├── src/
│   ├── test/
│   ├── prisma/
│   └── docker-compose.yml
├── esp32_firmware/         # ESP32 Arduino/ESP-IDF code
│   ├── src/
│   │   ├── drivers/       # Router drivers
│   │   ├── mqtt/          # MQTT client
│   │   ├── policy/        # Local policy engine
│   │   └── main.cpp
│   └── platformio.ini
├── flutter_app/            # Parent dashboard
│   ├── lib/
│   │   ├── screens/
│   │   ├── widgets/
│   │   ├── services/
│   │   └── models/
│   └── test/
└── docs/
```

---

## Code Conventions

### General

- Use **English** for all code, comments, and commit messages.
- Use **Arabic** for user-facing content (app text, notifications) where appropriate.
- Follow **existing patterns** in the codebase — consistency over personal preference.

### NestJS (TypeScript)

- Use ESLint + Prettier.
- Naming: `camelCase` for variables/functions, `PascalCase` for classes/interfaces.
- Modules organized by domain: `auth/`, `profiles/`, `devices/`, `routers/`.
- Business logic in **services**, not controllers.
- DTO validation with `class-validator`.
- Use Prisma for database access.

### Flutter (Dart)

- Use `flutter_lints` package.
- State management: Riverpod or Bloc (pick one, stay consistent).
- Widgets broken into small, reusable components.
- API client in `lib/services/api_client.dart`.
- Models with `json_serializable`.

### ESP32 (C++)

- Use PlatformIO for builds.
- Class naming: `PascalCase`.
- Function/variable naming: `camelCase`.
- Constants: `UPPER_SNAKE_CASE`.
- No dynamic allocation in hot paths.
- Use `std::` types over Arduino types where possible.
- Router drivers inherit from `IRouterDriver`.

---

## Git Workflow

### Branching

```
main         ── Production-ready
  └── develop    ── Integration branch
       ├── feat/xxx
       ├── fix/xxx
       └── refactor/xxx
```

### Commit Messages

```
<type>: <short description>

feat: Add quota management API endpoints
fix: Handle router login timeout properly
docs: Add sequence diagram for device approval flow
refactor: Extract HTTP client into reusable class
```

### Pull Requests

- PR title matches commit convention.
- Include `Closes #issue` if applicable.
- At least 1 reviewer approval required.
- Squash merge to `develop`.

---

## Testing

### Backend

```bash
pnpm run test          # Unit tests
pnpm run test:e2e      # End-to-end tests
pnpm run test:cov      # Coverage report
```

### Flutter

```bash
flutter test
flutter test --coverage
```

### ESP32

- Manual testing physical router interactions.
- Unit tests for policy engine logic (host machine test build).

---

## Documentation

- All PRs with new features must include or update relevant docs.
- Diagrams use **Mermaid** syntax (renders automatically in Markdown).
- Keep `supported_routers.md` up to date with actual test results.
- Update `non_goals.md` if scope changes are approved.
