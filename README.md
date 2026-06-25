# EMO — Family Internet Manager

> Low-cost smart device for family home internet management — Control, Monitor, Protect.

![Status](https://img.shields.io/badge/status-MVP-blue)
![Platform](https://img.shields.io/badge/platform-ESP32%20%7C%20NestJS%20%7C%20Flutter-green)

---

## Overview

EMO is a **Home Internet Management Platform** built on three layers:

| Layer | Technology | Role |
|---|---|---|
| **EMO Device** | ESP32-WROOM-32 + W5500 | Router Control Agent |
| **Cloud** | NestJS + PostgreSQL + EMQX | Backend + MQTT Broker |
| **Parent App** | Flutter (Mobile) | Dashboard & Control |

ESP32 does **not** pass traffic, inspect packets, or act as a gateway. It controls the router via HTTP APIs.

---

## Quick Start

```bash
git clone https://github.com/your-org/emo-home-security.git
cd emo-home-security

# Backend
cd cloud
cp ../.env.example .env
npm install
npm run start:dev

# Flutter App
cd ../flutter_app
flutter pub get
flutter run
```

---

## Repo Structure

```
emo-home-security/
├── cloud/                  # NestJS Backend
├── esp32_firmware/         # ESP32 Arduino / ESP-IDF
├── flutter_app/            # Parent Dashboard (Flutter)
├── docs/                   # All documentation
│   ├── diagrams/           # Mermaid UML diagrams
│   ├── project_description.md
│   ├── system_architecture.md
│   ├── security_model.md
│   ├── api_design.md
│   ├── database_schema.md
│   ├── router_integration_guide.md
│   ├── supported_routers.md
│   ├── non_goals.md
│   ├── developer_guidelines.md
│   └── project_roadmap.md
└── .env.example
```

---

## Documentation

| Document | Description |
|---|---|
| [Project Description](docs/project_description.md) | Vision, problem, solution, features |
| [System Architecture](docs/system_architecture.md) | Three-layer architecture |
| [Security Model](docs/security_model.md) | Encryption, auth, tamper detection |
| [API Design](docs/api_design.md) | REST endpoints + MQTT topics |
| [Database Schema](docs/database_schema.md) | Tables & relationships |
| [Router Integration](docs/router_integration_guide.md) | IRouterDriver, supported APIs |
| [Supported Routers](docs/supported_routers.md) | Device compatibility table |
| [Non-Goals](docs/non_goals.md) | What MVP explicitly excludes |
| [Roadmap](docs/project_roadmap.md) | Phases 1–5 |
| [Developer Guidelines](docs/developer_guidelines.md) | Setup, conventions, workflow |

---

## License

All Rights Reserved © Eng.Emam AbdullAziz
