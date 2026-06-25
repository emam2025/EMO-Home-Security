# Security Model

## Overview

EMO handles sensitive data — router credentials, family information, and network control. Security is layered across device, cloud, and communication.

---

## 1. Router Credential Storage

### On ESP32 (NVS)

- Router credentials encrypted with AES-256-GCM before storage.
- Encryption key derived from device unique ID (MAC) + pairing secret.
- Plaintext credentials never written to flash logs.
- NVS partition access restricted.

### In Cloud (PostgreSQL)

- Router credentials encrypted at rest using AES-256.
- Encryption key stored in environment variable (`ENCRYPTION_KEY`).
- Decryption only occurs server-side when pushing to device.
- Cloud never exposes raw credentials to API responses.

---

## 2. Communication Security

| Channel | Protection |
|---|---|
| Device ↔ Cloud (MQTT) | MQTT over TLS 1.2+ (port 8883) |
| Cloud ↔ App (HTTPS) | TLS 1.2+ |
| Device ↔ Router (HTTP) | Over local LAN (isolated network) |
| App ↔ Cloud (WebSocket) | WSS (WebSocket Secure) |

---

## 3. Authentication

### JWT (Cloud API)

- Token-based auth for all API requests.
- Access token: short-lived (15 min).
- Refresh token: long-lived (7 days), rotate on use.
- Payload includes `userId`, `homeId`, `role`.

### Device Pairing

- Each device paired to one home using a pairing code.
- Pairing code generated server-side, displayed during setup.
- Device sends pairing code + MAC address to claim ownership.
- MQTT credentials are device-specific and rotated periodically.

---

## 4. Tamper Detection

EMO monitors for:

| Event | Detection Method | Action |
|---|---|---|
| Device offline | MQTT last-will + heartbeat timeout | Alert parent |
| Router reboot | ESP32 boot counter mismatch / reachability check | Re-apply policies |
| Router factory reset | Login failure after known-good credentials | Trigger recovery flow |
| Router config change | Periodic config hash comparison | Alert + restore |
| Physical tamper | (Future) Enclosure switch | Alert + clear credentials |

---

## 5. Router Recovery System

When a factory reset is detected:

```
Factory Reset Detected
        │
        ▼
Try Login with stored credentials
        │
        ├── Success → Re-apply policies → Normal operation
        │
        └── Failure → Begin Recovery:
                │
                ▼
        1. Login with factory credentials (from stored model defaults)
        2. Reconfigure WAN, WiFi, DNS settings
        3. Restore EMO management user
        4. Restore DNS settings
        5. Notify parent of recovery
```

**Stored per router:**
- Router model & firmware version
- Current management credentials (encrypted)
- Factory default credentials (encrypted)
- Router fingerprint (MAC, serial)
- Last known good configuration snapshot

---

## 6. Anti-Tamper Strategy

| Threat | Mitigation |
|---|---|
| Credential theft | Encrypted storage, no exposure in API |
| Man-in-the-middle | TLS for all external communication |
| Replay attacks | MQTT client ID binding, JWT expiry |
| Unauthorized device pairing | Pairing code + MAC binding |
| Cloud data breach | Encrypted PII and credentials at rest |
| Physical device theft | Credentials encrypted, no plaintext access |

---

## 7. Secure Development Practices

- No secrets in code (use `.env` or vault).
- Regular dependency audits (`npm audit`).
- ESP32 firmware signed and versioned.
- API rate limiting on auth endpoints.
- MQTT ACLs per device topic.
