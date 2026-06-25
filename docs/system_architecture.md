# System Architecture

## Overview

EMO follows a **three-layer architecture**: Device → Cloud → App.

The ESP32 device acts exclusively as a **Router Control Agent** — it does not pass traffic, inspect packets, or function as a network gateway.

```
┌─────────────────────────────────────────────────────────────┐
│                     Parent Dashboard                         │
│                       (Flutter App)                          │
│                   Mobile / Tablet / Web                      │
└──────────────────────────┬──────────────────────────────────┘
                           │ HTTPS / WebSocket
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                      Cloud Layer                             │
│              ┌─────────────────────────────┐                │
│              │      NestJS (API Server)     │                │
│              │  PostgreSQL │ Redis │ EMQX   │                │
│              └─────────────────────────────┘                │
└──────────────────────────┬──────────────────────────────────┘
                           │ MQTT (TLS)
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                      EMO Device                              │
│               ESP32-WROOM-32 + W5500 Ethernet                │
│                    Router Control Agent                       │
│  ┌──────────────────────────────────────────────────────┐    │
│  │  Policy Engine  │  MQTT Client  │  Router Driver     │    │
│  │  (Local rules)  │  (Cloud sync) │  (HTTP to router)  │    │
│  └──────────────────────────────────────────────────────┘    │
└──────────────────────────┬──────────────────────────────────┘
                           │ HTTP (Router APIs)
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                     Home Router                               │
│             Huawei / ZTE / TP-Link / etc.                    │
└─────────────────────────────────────────────────────────────┘
```

---

## Layer Details

### 1. EMO Device (ESP32)

**Hardware:**
- ESP32-WROOM-32
- W5500 Ethernet Module (stable wired connection)
- 8MB Flash minimum
- USB-C Power
- Status LED

**Software Responsibilities:**
- Router communication via HTTP (login, get devices, block/unblock, set DNS)
- MQTT communication with Cloud (status, usage, devices, alerts)
- Local Policy Engine (apply rules even if cloud is unreachable)
- Device detection polling
- Configuration synchronization

**Non-Responsibilities (MVP):**
- No traffic passing
- No packet inspection
- No gateway mode
- No bandwidth measurement
- No content filtering on-device

### 2. Cloud Layer

**Technology Stack:**
- Node.js / NestJS (REST API + WebSocket)
- PostgreSQL (relational data)
- Redis (caching, session, rate limiting)
- EMQX (MQTT Broker)

**Responsibilities:**
- User authentication & JWT management
- Device registry & pairing
- Policy storage & synchronization
- Push notifications (FCM)
- Real-time updates via WebSocket
- MQTT message routing

**Deployment:**
- VPS (any Linux host)
- Docker Compose (NestJS + PostgreSQL + Redis + EMQX)

### 3. Parent Dashboard (Flutter)

**Platforms:**
- Android
- iOS
- (Future: Web)

**Screens:**
- Login / Register
- Dashboard (overview of all profiles)
- Profiles (manage family members)
- Devices (view, approve, block)
- Usage (per profile consumption)
- Schedules (time windows)
- Quotas (data limits)
- Notifications (alerts, approval requests)
- Settings
- Router (connection status, model info)

---

## Data Flow

### Read Path (Usage → Parent)
```
Router ──HTTP──> ESP32 ──MQTT──> Cloud ──WS/HTTPS──> Flutter App
```

### Write Path (Parent → Router)
```
Flutter App ──HTTPS──> Cloud ──MQTT──> ESP32 ──HTTP──> Router
```

---

## MQTT Communication

| Direction | Topic | Payload |
|---|---|---|
| Device → Cloud | `emo/{deviceId}/status` | Device online/offline, uptime |
| Device → Cloud | `emo/{deviceId}/devices` | List of connected devices |
| Device → Cloud | `emo/{deviceId}/usage` | Per-device traffic deltas |
| Device → Cloud | `emo/{deviceId}/alerts` | New device, tamper, error |
| Cloud → Device | `emo/{deviceId}/commands` | Block, unblock, set DNS, reboot |
| Cloud → Device | `emo/{deviceId}/policies` | Policy update payload |
