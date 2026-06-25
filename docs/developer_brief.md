# EMO Family Internet Manager
## One-Page Developer Brief

---

## Product Goal

Build a low-cost home internet management system that lets a parent control family internet access by profile, quota, schedule, and device approval, with remote access through a cloud dashboard.

---

## MVP Scope

The first release must include only the core control loop:

- ESP32 as a Router Control Agent only
- Cloud backend for authentication, policies, and real-time sync
- Flutter parent app for remote control and monitoring
- Router driver v1 for a single supported router model
- Device discovery, block/unblock, quota rules, and schedules
- Tamper detection and basic recovery behavior

---

## Non-Goals

Do not implement in the MVP:

- DPI / packet inspection
- Traffic sniffing
- Gateway / proxy mode
- Bandwidth measurement on ESP32
- Media server / NAS
- IoT home automation
- AI classification

---

## Target Architecture

```
ESP32 → MQTT/TLS → Cloud Backend → Flutter App
```

---

## ESP32 Responsibilities

- Connect to router management interface
- Apply router actions: block, unblock, DNS, whitelist
- Poll router state and connected devices
- Enforce local policy state
- Send heartbeat, events, and alerts to cloud
- Store credentials and device state securely
- Support OTA updates

---

## Cloud Responsibilities

- Authentication (JWT)
- Home / profile / device / quota / schedule management
- MQTT broker integration
- Notifications and live sync
- Persistent storage in PostgreSQL

---

## Flutter App Responsibilities

- Login and dashboard
- Family profiles
- Device list and approval
- Quota and schedule editing
- Alerts and usage visibility
- Remote actions from خارج المنزل

---

## Router Driver Strategy

Use a router abstraction layer:

- `login()`
- `logout()`
- `getConnectedDevices()`
- `blockDevice(mac)`
- `unblockDevice(mac)`
- `setDNS()`
- `enableWhitelist()`
- `disableWhitelist()`
- `getStatistics()` if supported

MVP support should start with one router model only (Huawei HG8145V5 recommended), then expand later.

---

## Data Model

Core entities:

- Users
- Homes
- Profiles
- Devices
- Routers
- QuotaRules
- Schedules
- UsageLogs
- Notifications
- Alerts
- Policies

---

## Key Implementation Rules

- ESP32 must not act as a gateway
- MQTT must use TLS
- Router credentials must be encrypted
- Usage records must map clearly from device → profile
- All policy updates must be recoverable after restart
- Keep the first release focused on control, not analytics

---

## Success Criteria for MVP

The MVP is successful if a parent can:

- Log in from outside the home
- See family devices and profiles
- Assign a quota and schedule to each profile
- Block/unblock a device remotely
- Watch the router policy stay enforced by the ESP32 agent
- Receive tamper/offline alerts

---

## Delivery Sequence

1. Cloud auth + database + MQTT
2. ESP32 connectivity + registration
3. Router driver v1
4. Device discovery + profile assignment
5. Quota + schedule enforcement
6. Parent dashboard
7. Tamper detection + OTA hardening

---

## Final Product Positioning

EMO is a **home internet governance system**, not just a parental-control app.