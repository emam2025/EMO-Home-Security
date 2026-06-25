# Project Description

## Product Vision

EMO is a low-cost smart device that manages home internet for families by controlling consumption, screen time, connected devices, and child safety — all manageable remotely.

The core idea is not just "Parental Control."

It is a **Home Internet Management Platform** that lets a parent know:

- Who is consuming the data plan.
- Who exceeded allowed time.
- Who connected to the network.
- Who is trying to bypass the rules.

All from anywhere outside the home.

---

## Business Problem

1. Data plan drained by children and teenagers.
2. No fair distribution of bandwidth among family members.
3. Internet usage outside permitted hours.
4. Access to inappropriate content.
5. Unknown devices connecting to the home network.
6. No visibility into per-person internet consumption.

---

## Solution

A small ESP32-based device that sits next to the home router and acts as a **Router Control Agent**.

The device does **not** pass traffic, inspect packets, or act as a gateway. It controls the router via HTTP APIs.

**EMO responsibilities:**
- Read connected devices from the router.
- Apply policies (block, quota, schedule).
- Change router DNS to family-safe providers.
- Send data to the cloud.
- Receive commands from the cloud.

---

## User Types

| User | Permissions |
|---|---|
| **Parent** | Full control — manage profiles, quotas, schedules, devices, alerts |
| **Child** | View own usage only — remaining quota, remaining time, request more |
| **Admin** | System management — device registry, user management, support |

---

## Main Features

### 1. Family Profiles
Each family member has a profile. Each profile owns devices, a quota, and a schedule.

### 2. Quota Management
Monthly data cap per profile. When consumed = quota, internet is blocked.

### 3. Time Management
Allowed time window per profile (e.g. 4 PM → 10 PM). Outside schedule = blocked.

### 4. Safe Internet
DNS-based protection using Cloudflare Family or OpenDNS Family.
Blocks adult content, gambling, malware, phishing.

### 5. Device Approval
Unknown devices are blocked. Parent receives alert and can approve/reject.

### 6. Remote Control
Parent can pause internet, resume internet, increase quota, or add bonus time from anywhere.

---

## MVP Scope

1. Router Integration (Huawei HG8145V5, HG8245, ZTE H188A)
2. Device Detection
3. User Profiles
4. Monthly Quotas
5. Time Schedules
6. Block/Unblock
7. Parent Dashboard (Flutter)
8. MQTT Communication
9. Notifications

Everything else is out of scope for MVP.
