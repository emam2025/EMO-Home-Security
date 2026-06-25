# Supported Routers

## Phase 1 Routers (MVP)

| Manufacturer | Model | Status | Notes |
|---|---|---|---|
| Huawei | HG8145V5 | ✅ Supported | Fully tested |
| Huawei | HG8245 | ✅ Supported | Fully tested |
| ZTE | H188A | ✅ Supported | Fully tested |

## Phase 2 (Planned)

| Manufacturer | Model | Status | Notes |
|---|---|---|---|
| TP-Link | Archer C6 | 🔄 Planned | Driver development pending |
| TP-Link | Archer C7 | 🔄 Planned | Driver development pending |
| ZTE | H288A | 🔄 Planned | Similar to H188A |

## Phase 3 (Backlog)

| Manufacturer | Model | Status | Notes |
|---|---|---|---|
| Huawei | HN8255W | 📋 Backlog | |
| TP-Link | Deco Series | 📋 Backlog | Mesh systems |
| Xiaomi | Router AX3000 | 📋 Backlog | |
| Linksys | EA Series | 📋 Backlog | |
| D-Link | DIR Series | 📋 Backlog | |

## Status Legend

| Icon | Meaning |
|---|---|
| ✅ Supported | Implemented, tested, production-ready |
| 🔄 Planned | Driver in development |
| 📋 Backlog | Planned for future phase |
| ❌ Not Supported | Will not be supported (see notes) |

## Requirements for Router Support

For a router to be supported, it must have:

1. A **web-based management interface** accessible via HTTP on LAN.
2. Ability to **list connected devices** (MAC + IP + hostname).
3. Ability to **block/unblock devices** by MAC address.
4. Ability to **change DNS settings**.
5. Ability to **enable/disable MAC whitelist** mode (optional but preferred).
6. Support for **session-based authentication** (cookie or token).

## Router Selection Guide

> If your router is not listed, check if it exposes a management interface at `192.168.1.1` or `192.168.100.1`. Many ISP-provided routers share code with supported models.
