# API Design

## Base URL

```
Production:  https://api.emo.local/v1
Staging:     https://api.staging.emo.local/v1
```

## Authentication

All API requests require `Authorization: Bearer <jwt_token>` header.

---

## REST Endpoints

### Auth

| Method | Path | Description |
|---|---|---|
| POST | `/auth/register` | Create parent account |
| POST | `/auth/login` | Login, returns JWT pair |
| POST | `/auth/refresh` | Refresh access token |
| POST | `/auth/logout` | Invalidate refresh token |

### Homes

| Method | Path | Description |
|---|---|---|
| POST | `/homes` | Create home |
| GET | `/homes/{homeId}` | Get home details |
| PATCH | `/homes/{homeId}` | Update home |
| DELETE | `/homes/{homeId}` | Delete home |

### Devices (EMO Hardware)

| Method | Path | Description |
|---|---|---|
| POST | `/homes/{homeId}/devices` | Pair new EMO device |
| GET | `/homes/{homeId}/devices` | List paired EMO devices |
| GET | `/homes/{homeId}/devices/{deviceId}` | Get device status |
| PATCH | `/homes/{homeId}/devices/{deviceId}` | Update device config |
| DELETE | `/homes/{homeId}/devices/{deviceId}` | Unpair device |

### Profiles (Family Members)

| Method | Path | Description |
|---|---|---|
| POST | `/homes/{homeId}/profiles` | Create profile (Ahmed, Mariam, etc.) |
| GET | `/homes/{homeId}/profiles` | List all profiles |
| GET | `/homes/{homeId}/profiles/{profileId}` | Get profile details |
| PATCH | `/homes/{homeId}/profiles/{profileId}` | Update profile |
| DELETE | `/homes/{homeId}/profiles/{profileId}` | Delete profile |

### Routers

| Method | Path | Description |
|---|---|---|
| POST | `/homes/{homeId}/routers` | Register router |
| GET | `/homes/{homeId}/routers` | List routers |
| GET | `/homes/{homeId}/routers/{routerId}` | Get router details |
| PATCH | `/homes/{homeId}/routers/{routerId}` | Update router config |
| DELETE | `/homes/{homeId}/routers/{routerId}` | Remove router |

### Quota Rules

| Method | Path | Description |
|---|---|---|
| POST | `/homes/{homeId}/profiles/{profileId}/quota` | Set quota rule |
| GET | `/homes/{homeId}/profiles/{profileId}/quota` | Get quota rule |
| PATCH | `/homes/{homeId}/profiles/{profileId}/quota` | Update quota |
| DELETE | `/homes/{homeId}/profiles/{profileId}/quota` | Remove quota |
| POST | `/homes/{homeId}/profiles/{profileId}/quota/bonus` | Add bonus data |

### Schedules

| Method | Path | Description |
|---|---|---|
| POST | `/homes/{homeId}/profiles/{profileId}/schedules` | Create schedule |
| GET | `/homes/{homeId}/profiles/{profileId}/schedules` | List schedules |
| PATCH | `/homes/{homeId}/profiles/{profileId}/schedules/{scheduleId}` | Update schedule |
| DELETE | `/homes/{homeId}/profiles/{profileId}/schedules/{scheduleId}` | Delete schedule |

### Network Devices (Connected to Router)

| Method | Path | Description |
|---|---|---|
| GET | `/homes/{homeId}/network-devices` | List all detected devices |
| PATCH | `/homes/{homeId}/network-devices/{mac}` | Approve / assign to profile |
| DELETE | `/homes/{homeId}/network-devices/{mac}` | Block / remove |

### Usage

| Method | Path | Description |
|---|---|---|
| GET | `/homes/{homeId}/usage` | Aggregate usage (all profiles) |
| GET | `/homes/{homeId}/profiles/{profileId}/usage` | Per-profile usage |
| GET | `/homes/{homeId}/usage/history?from=&to=` | Historical usage |

### Commands

| Method | Path | Description |
|---|---|---|
| POST | `/homes/{homeId}/devices/{deviceId}/commands` | Send command to EMO device |
| GET | `/homes/{homeId}/devices/{deviceId}/commands` | Command history |

**Command payload:**
```json
{
  "command": "block_device" | "unblock_device" | "set_dns" | "reboot" |
               "pause_internet" | "resume_internet" | "sync_policies",
  "params": {}
}
```

### Notifications

| Method | Path | Description |
|---|---|---|
| GET | `/notifications` | List notifications |
| PATCH | `/notifications/{id}` | Mark as read |
| POST | `/notifications/register-device` | Register FCM token |

---

## MQTT Topics

### Device → Cloud (PUB)

| Topic | Payload | Frequency |
|---|---|---|
| `emo/{deviceId}/status` | `{ "online": true, "uptime": 3600, "rssi": -45 }` | Every 60s |
| `emo/{deviceId}/devices` | `{ "devices": [{ "mac": "...", "ip": "...", "hostname": "..." }] }` | Every 120s / on change |
| `emo/{deviceId}/usage` | `{ "profileId": "...", "consumedMb": 500, "period": "2025-06" }` | Every 300s |
| `emo/{deviceId}/alerts` | `{ "type": "new_device" \| "tamper" \| "router_offline" \| "quota_exhausted", "data": {} }` | On event |

### Cloud → Device (SUB)

| Topic | Payload | Description |
|---|---|---|
| `emo/{deviceId}/commands` | `{ "command": "...", "params": {}, "id": "uuid" }` | Remote action |
| `emo/{deviceId}/policies` | `{ "profiles": [...], "updatedAt": "..." }` | Full policy sync |

### Device Last-Will

```
Topic: emo/{deviceId}/status
Payload: { "online": false }
Retain: true
```

---

## WebSocket Events

### Parent App → Cloud

| Event | Payload | Description |
|---|---|---|
| `subscribe:home` | `{ homeId }` | Subscribe to home updates |
| `command:send` | `{ deviceId, command, params }` | Send command via WS |

### Cloud → Parent App

| Event | Payload | Description |
|---|---|---|
| `device:status` | `{ deviceId, online, uptime }` | Real-time device status |
| `device:new` | `{ mac, ip, hostname }` | New unknown device detected |
| `alert` | `{ type, data }` | Real-time alert |
| `usage:update` | `{ profileId, consumedMb }` | Usage tick |
