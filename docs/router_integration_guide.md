# Router Integration Guide

## Architecture

EMO controls routers via HTTP APIs exposed by the router's management interface. All communication is over the local LAN — no cloud API calls to the router.

```
┌─────────────┐     HTTP (LAN)     ┌──────────────┐
│  EMO Device  │ ────────────────▶  │    Router    │
│  (ESP32)     │ ◀────────────────  │  (Huawei /   │
│  Router      │     Response       │   ZTE / ...) │
│  Driver      │                    │              │
└─────────────┘                    └──────────────┘
```

---

## IRouterDriver Interface

Every router driver implements this interface:

```typescript
interface IRouterDriver {
  login(): Promise<boolean>;
  logout(): Promise<void>;
  getDevices(): Promise<RouterDevice[]>;
  blockDevice(mac: string): Promise<boolean>;
  unblockDevice(mac: string): Promise<boolean>;
  setDNS(primary: string, secondary: string): Promise<boolean>;
  enableWhitelist(): Promise<boolean>;
  disableWhitelist(): Promise<boolean>;
  getStatistics(): Promise<RouterStatistics>;
  reboot(): Promise<boolean>;
}
```

### RouterDevice

```typescript
interface RouterDevice {
  mac: string;
  ip: string;
  hostname: string;
  vendor?: string;
  signalStrength?: number;
  isOnline: boolean;
}
```

### RouterStatistics

```typescript
interface RouterStatistics {
  totalDevices: number;
  onlineDevices: number;
  uptime: number;
  cpuUsage?: number;
  memoryUsage?: number;
}
```

---

## Router Driver Implementations

### Huawei HG8145V5

| Property | Value |
|---|---|
| Default IP | 192.168.1.1 |
| Default Port | 80 |
| Auth | POST to `/login.cgi` |
| Session | Cookie-based (`Cookie: sid=<session>`) |

**Endpoints used:**
- `POST /login.cgi` — Login with username/password
- `GET / deviceInformation` — Device list
- `POST /wlmacflt.cgi` — MAC filtering (block/unblock)
- `POST / dnsset.cgi` — DNS settings
- `GET /status.cgi` — Router statistics

### Huawei HG8245

| Property | Value |
|---|---|
| Default IP | 192.168.100.1 |
| Default Port | 80 |
| Auth | POST to `/login.cgi` |
| Session | Cookie-based |

**Endpoints used:**
- Similar to HG8145V5 with minor URL differences
- Device list at `/lanstatisticdevlist.cgi`
- MAC filtering at `/wlmacflt.cgi`

### ZTE H188A

| Property | Value |
|---|---|
| Default IP | 192.168.1.1 |
| Default Port | 80 |
| Auth | POST to `/login` |
| Session | Cookie + CSRF token |

**Endpoints used:**
- `POST /login` — Login
- `GET /data/wireless-devices.json` — Device list
- `POST /goform/blockDevice` — Block device
- `POST /goform/setDns` — DNS settings

---

## Driver Registration

```cpp
class RouterDriverFactory {
public:
  static IRouterDriver* create(const RouterConfig& config) {
    switch (config.model) {
      case RouterModel::HG8145V5:
        return new HuaweiHG8145V5Driver(config);
      case RouterModel::HG8245:
        return new HuaweiHG8245Driver(config);
      case RouterModel::ZTE_H188A:
        return new ZTEH188ADriver(config);
      default:
        return nullptr; // Unsupported
    }
  }
};
```

---

## ESP32 Implementation Notes

### HTTP Client

- Use `HTTPClient` from ESP32 Arduino core.
- Set connection timeout to 5 seconds.
- Retry failed requests up to 3 times with exponential backoff.
- Store session cookies in memory (not in NVS).

### Error Handling

| Error | Action |
|---|---|
| Connection timeout | Retry x3, then report offline via MQTT |
| Login failed (wrong creds) | Notify parent, do not retry |
| Login failed (factory reset) | Trigger Router Recovery flow |
| HTTP 401/403 | Re-login and retry |
| HTTP 5xx | Retry x3, then skip cycle |

### Polling Cycle

```
[Loop Start]
  │
  ├─ Poll device list (every 120s)
  ├─ Apply pending policies (if any)
  ├─ Check router reachability (every 30s)
  ├─ Send status update via MQTT (every 60s)
  │
[Loop End ~300ms total]
```

---

## Adding a New Router

1. Create a new driver class implementing `IRouterDriver`.
2. Add the model to `RouterModel` enum.
3. Add the entry to `RouterDriverFactory::create()`.
4. Add the driver files to the firmware build.
5. Update `supported_routers.md`.
6. Test on actual hardware.
