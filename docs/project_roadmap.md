# Project Roadmap

## Phase 1 — Internet Management MVP

**Goal:** Ship the core product — a working router control agent with profiles, quotas, schedules, and a parent dashboard.

| Feature | Status |
|---|---|
| Router Integration (Huawei HG8145V5, HG8245, ZTE H188A) | ✅ |
| Device Detection (poll router for connected devices) | ✅ |
| Family Profiles | ✅ |
| Monthly Quotas (block when exhausted) | ✅ |
| Time Schedules (block outside allowed window) | ✅ |
| Block/Unblock Devices (MAC-based) | ✅ |
| Parent Dashboard (Flutter) | ✅ |
| MQTT Communication (Device ↔ Cloud) | ✅ |
| Push Notifications (new device, quota exhausted) | ✅ |

**Explicitly excluded (see [non_goals.md](non_goals.md)):**
- No DPI, no traffic inspection, no gateway mode

**Estimated timeline:** 12–16 weeks

---

## Phase 2 — Advanced Security

**Goal:** Strengthen security, improve router support, add analytics.

| Feature | Priority |
|---|---|
| DNS Filtering (Cloudflare Family, OpenDNS Family) | High |
| Device Analytics (history, trends) | Medium |
| Router Recovery System (factory reset auto-restore) | High |
| Tamper Detection (offline alerts, config change alerts) | High |
| Additional Router Drivers (TP-Link Archer C6/C7) | Medium |
| Web Dashboard (responsive web version) | Low |

**Estimated timeline:** 8–12 weeks after Phase 1

---

## Phase 3 — EMO Home

**Goal:** Expand beyond internet management into smart home.

| Feature | Priority |
|---|---|
| IoT Device Integration | Medium |
| Smart Plug Control (energy cutoff by schedule) | Medium |
| Motion / Door Sensors | Low |
| Home Automation Rules | Low |
| Scene Creation | Low |

**Estimated timeline:** 12–16 weeks after Phase 2

---

## Phase 4 — EMO Energy

**Goal:** Energy monitoring and optimization.

| Feature | Priority |
|---|---|
| Per-Device Energy Monitoring | Medium |
| Power Consumption Analytics | Medium |
| Optimization Recommendations | Low |
| Energy Budgets & Alerts | Low |

**Estimated timeline:** 8–12 weeks after Phase 3

---

## Phase 5 — EMO AI Home OS

**Goal:** Intelligence layer for the entire home.

| Feature | Priority |
|---|---|
| AI Usage Recommendations | Medium |
| Automated Schedule Optimization | Medium |
| Anomaly Detection (unusual usage patterns) | Medium |
| Family Behavior Insights | Low |
| Predictive Quota Management | Low |

**Estimated timeline:** 12–16 weeks after Phase 4

---

## Milestone Summary

```
Phase 1 ───▶ MVP Launch
    │
Phase 2 ───▶ Secure & Expand
    │
Phase 3 ───▶ Smart Home
    │
Phase 4 ───▶ Energy
    │
Phase 5 ───▶ AI OS
```

Each phase builds on the previous. No phase starts until the previous phase is stable in production.
