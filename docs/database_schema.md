# Database Schema

## Technology

PostgreSQL 15+

---

## Entity Relationship Summary

```
users ──── homes ──── profiles ──── devices
              │            │
              │            ├── quota_rules
              │            ├── schedules
              │            └── usage_logs
              │
              ├── routers
              ├── network_devices
              └── notifications
```

---

## Tables

### users

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| email | VARCHAR(255) | Unique, not null |
| password_hash | VARCHAR(255) | bcrypt |
| name | VARCHAR(255) | |
| role | ENUM('parent', 'admin') | Default: parent |
| fcm_token | TEXT | Push notification token |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### homes

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| name | VARCHAR(255) | e.g. "My Home" |
| owner_id | UUID | FK → users.id |
| timezone | VARCHAR(50) | e.g. "Asia/Riyadh" |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### home_members

| Column | Type | Notes |
|---|---|---|
| home_id | UUID | FK → homes.id |
| user_id | UUID | FK → users.id |
| role | ENUM('parent', 'child', 'admin') | |

### profiles

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| home_id | UUID | FK → homes.id |
| name | VARCHAR(255) | e.g. "Ahmed" |
| avatar | VARCHAR(50) | Emoji or icon key |
| is_active | BOOLEAN | Default: true |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### routers

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| home_id | UUID | FK → homes.id |
| model | VARCHAR(100) | e.g. "HG8145V5" |
| manufacturer | VARCHAR(50) | e.g. "Huawei" |
| ip_address | INET | Router LAN IP |
| mac_address | MACADDR | Router MAC |
| serial_number | VARCHAR(255) | |
| firmware_version | VARCHAR(100) | |
| credentials_encrypted | TEXT | AES-256 encrypted JSON |
| factory_credentials_encrypted | TEXT | AES-256 encrypted JSON |
| config_snapshot | JSONB | Last known good config |
| fingerprint | VARCHAR(255) | Router unique ID |
| is_managed | BOOLEAN | Default: true |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### devices (EMO Hardware)

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| home_id | UUID | FK → homes.id |
| router_id | UUID | FK → routers.id |
| mac_address | MACADDR | ESP32 MAC |
| firmware_version | VARCHAR(50) | |
| mqtt_username | VARCHAR(255) | Device-specific |
| mqtt_password_encrypted | TEXT | |
| pairing_code | VARCHAR(10) | Temporary |
| paired_at | TIMESTAMP | |
| last_seen_at | TIMESTAMP | |
| is_online | BOOLEAN | |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### network_devices (Connected to Router)

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| home_id | UUID | FK → homes.id |
| router_id | UUID | FK → routers.id |
| profile_id | UUID | FK → profiles.id (nullable) |
| mac_address | MACADDR | Device MAC |
| ip_address | INET | Current IP |
| hostname | VARCHAR(255) | |
| vendor | VARCHAR(100) | OUI lookup |
| status | ENUM('pending', 'approved', 'blocked', 'unknown') | |
| first_seen_at | TIMESTAMP | |
| last_seen_at | TIMESTAMP | |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### quota_rules

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| profile_id | UUID | FK → profiles.id |
| quota_gb | DECIMAL(10,2) | Monthly limit in GB |
| consumed_gb | DECIMAL(10,2) | Current month consumption |
| period | VARCHAR(7) | "2025-06" (YYYY-MM) |
| action_on_exhaust | ENUM('block', 'throttle') | Default: block |
| reset_day | INT | Day of month to reset |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### schedules

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| profile_id | UUID | FK → profiles.id |
| day_of_week | INT | 0=Sun .. 6=Sat (or NULL = all) |
| start_time | TIME | e.g. "16:00" |
| end_time | TIME | e.g. "22:00" |
| is_active | BOOLEAN | Default: true |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### usage_logs

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| profile_id | UUID | FK → profiles.id |
| network_device_id | UUID | FK → network_devices.id |
| bytes_downloaded | BIGINT | |
| bytes_uploaded | BIGINT | |
| logged_at | TIMESTAMP | |
| created_at | TIMESTAMP | |

### policies

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| home_id | UUID | FK → homes.id |
| dns_provider | ENUM('default', 'cloudflare_family', 'opendns_family', 'custom') | |
| custom_dns_primary | INET | |
| custom_dns_secondary | INET | |
| whitelist_enabled | BOOLEAN | |
| new_device_default_action | ENUM('approve', 'block', 'notify') | |
| created_at | TIMESTAMP | |
| updated_at | TIMESTAMP | |

### notifications

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| user_id | UUID | FK → users.id |
| type | VARCHAR(50) | new_device, quota_exhausted, tamper, etc. |
| title | VARCHAR(255) | |
| body | TEXT | |
| data | JSONB | Extra payload |
| is_read | BOOLEAN | Default: false |
| created_at | TIMESTAMP | |

### events

| Column | Type | Notes |
|---|---|---|
| id | UUID | PK |
| device_id | UUID | FK → devices.id |
| type | VARCHAR(50) | command_sent, policy_applied, error, etc. |
| payload | JSONB | |
| created_at | TIMESTAMP | |

---

## Indexes

```sql
CREATE INDEX idx_network_devices_mac ON network_devices(mac_address);
CREATE INDEX idx_usage_logs_profile ON usage_logs(profile_id, logged_at);
CREATE INDEX idx_notifications_user ON notifications(user_id, is_read);
CREATE INDEX idx_events_device ON events(device_id, created_at);
CREATE INDEX idx_quota_rules_period ON quota_rules(profile_id, period);
```

---

## Notes

- All UUIDs use `uuid_generate_v4()`.
- Timestamps use `TIMESTAMPTZ` (timezone-aware).
- Soft deletes are not used — records are hard deleted or anonymized after 90 days.
- Quota consumption is calculated server-side from `usage_logs`.
