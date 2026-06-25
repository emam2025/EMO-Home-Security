# MVP Execution Plan — 8 Weeks

> EMO Family Internet Manager
> Realistic plan to reach a home-testable MVP

---

## Milestone 0 — Project Foundation (3–4 Days)

**Goal:** Set up development environment and basic infrastructure.

| Task | Details |
|---|---|
| **Repository Setup** | Create project structure: `Firmware/`, `Backend/`, `Mobile/`, `Docs/` |
| **Infrastructure** | GitHub repo, Docker Compose (PostgreSQL + EMQX + Redis), CI/CD via GitHub Actions (lint, unit tests, build verification) |

**Deliverables:**
- ✅ Organized project
- ✅ Development environments ready

---

## Milestone 1 — Cloud Core (Week 1)

**Goal:** Build a basic Backend.

### Backend
| Task | Sub-tasks |
|---|---|
| **Authentication** | Register, Login, JWT, Refresh Token |
| **Database** | Core tables: `users`, `homes`, `profiles`, `devices`, `routers` |
| **MQTT** | Set up EMQX, TLS, Define Topics |

**Deliverables:**
- ✅ User can create an account
- ✅ Database ready
- ✅ MQTT Broker running

---

## Milestone 2 — ESP32 Connectivity (Week 2)

**Goal:** Connect ESP32 to the cloud.

### Firmware
| Task | Details |
|---|---|
| **WiFi / Ethernet** | Connect, Reconnect, Heartbeat |
| **MQTT** | Publish, Subscribe, TLS |
| **Device Registration** | First run: ESP32 → Register Device → Cloud |

### Topics
| Topic | Direction |
|---|---|
| `emo/{deviceId}/status` | Device → Cloud |
| `emo/{deviceId}/events` | Device → Cloud |
| `emo/{deviceId}/commands` | Cloud → Device |

**Deliverables:**
- ✅ ESP32 shows Online
- ✅ Heartbeat every 30 seconds

---

## Milestone 3 — Router Driver V1 (Week 3)

**Goal:** Support only the first router.

### Target Router
Huawei HG8145V5

### Build
| Component | Functions |
|---|---|
| `IRouterDriver` | Basic interface |
| Implementation | `login()`, `getDevices()`, `blockDevice()`, `unblockDevice()`, `setDNS()` |

### Testing
- Read Device List
- Block Device
- Unblock Device

**Deliverables:**
- ✅ Actual router control

---

## Milestone 4 — Device Discovery (Week 4)

**Goal:** Discover and manage devices.

### Firmware
- Polling: every 60 seconds

### Backend
- Endpoints: `GET /devices`, `POST /assign-profile`

### App
- Screen: Connected Devices

**Deliverables:**
- ✅ View connected devices
- ✅ Link device to user

---

## Milestone 5 — User Profiles & Quotas (Week 5)

**Goal:** Implement the user system.

| Domain | Elements |
|---|---|
| **Profiles** | Father, Mother, Ahmed, Mariam |
| **Policies** | Monthly Quota per Profile |
| **Backend** | `quota_rules`, `usage_logs` |
| **App** | Profile Screen |

**Deliverables:**
- ✅ Create personal profiles
- ✅ Link devices to each user

---

## Milestone 6 — Scheduling Engine (Week 6)

**Goal:** Time-based control.

### Rules
Example: Ahmed — 4 PM → 10 PM

### Firmware
- Every minute: Check Schedule → Block / Unblock

**Deliverables:**
- ✅ Internet works within specified times

---

## Milestone 7 — Parent Dashboard (Week 7)

**Goal:** Real operational interface.

### Flutter
| Screen | Elements |
|---|---|
| **Dashboard** | Overview |
| **Devices** | Device list |
| **Profiles** | Profile management |
| **Usage** | Consumption |
| **Status** | Device and router status |

### Actions
- Pause Internet
- Resume Internet
- Add Time
- Add Quota

### Notifications
- New Device
- Offline Device
- Quota Exhausted

**Deliverables:**
- ✅ Usable application

---

## Milestone 8 — Security & Hardening (Week 8)

**Goal:** Stabilize the system for home testing.

| Domain | Details |
|---|---|
| **Tamper Detection** | Detect: Router Restart, Router Reset, EMO Offline |
| **Recovery** | Router Fingerprint, Credential Recovery, Policy Reapply |
| **OTA** | Firmware Update |

**Deliverables:**
- ✅ Stable MVP
- ✅ Ready for home testing

---

## Post-MVP Deferrals

### Phase 2
- ❌ Multiple routers
- ❌ DNS Timeline
- ❌ Advanced Analytics
- ❌ Child Portal
- ❌ Safe DNS Categories

### Phase 3
- ❌ Home Automation
- ❌ Smart Energy
- ❌ IoT Sensors
- ❌ AI Recommendations

---

## True MVP Success Definition

> If you can fully execute the following scenario:

1. The father opens the app
2. He sees the connected devices
3. He creates "Ahmed"
4. He links Ahmed's device
5. He sets: 30 GB, 4 PM → 10 PM
6. He presses Save
7. EMO applies the policy to the router
8. At 10 PM, internet is automatically stopped
9. The father sees the status from outside the home

**Then you have achieved a successful MVP ready for testing with real users before any further expansions.**
