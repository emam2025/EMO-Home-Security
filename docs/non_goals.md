# Non-Goals (MVP)

This document explicitly defines what is **out of scope** for the MVP.
It prevents scope creep and keeps the team focused on shipping a working product.

---

## Traffic & Network

| Item | Explanation |
|---|---|
| ❌ No Deep Packet Inspection (DPI) | ESP32 does not analyze packet contents. |
| ❌ No Packet Sniffing / Traffic Analysis | No pcap, no tcpdump, no packet-level inspection. |
| ❌ No Gateway Mode | ESP32 does not route traffic between networks. |
| ❌ No Proxy Mode | No HTTP/SOCKS proxy, no transparent proxy. |
| ❌ No Bandwidth Measurement | No per-device bandwidth counting on ESP32. |
| ❌ No On-Device Content Filtering | ESP32 does not filter HTTP/HTTPS content at the chip level. DNS filtering is done via router DNS settings (Cloudflare Family, OpenDNS Family) and remains a separate, planned feature. |
| ❌ No VPN / Tunneling | No WireGuard, OpenVPN, or any tunnel termination. |

---

## Hardware

| Item | Explanation |
|---|---|
| ❌ No Custom PCB in MVP | Development on ESP32 dev boards. |
| ❌ No PoE Support | USB-C power only for MVP. |
| ❌ No Battery Backup | Device runs on continuous power. |
| ❌ No LCD / Display | Status LED only. |

---

## Software

| Item | Explanation |
|---|---|
| ❌ No Mobile App for Children | Child portal is a web page, not a native app for MVP. |
| ❌ No Web Dashboard | Parent access is Flutter mobile app only. Web can be Phase 2. |
| ❌ No Multi-Language | MVP in Arabic/English toggle (or one language). |
| ❌ No Third-Party Integrations | No IFTTT, Home Assistant, Alexa, Google Home. |
| ❌ No IoT / Smart Home Features | Phase 3. |
| ❌ No Energy Monitoring | Phase 4. |
| ❌ No AI / Recommendations | Phase 5. |
| ❌ No Advanced Analytics | Charts and basic usage stats only. |

---

## Router Support

| Item | Explanation |
|---|---|
| ❌ No Universal Auto-Detection | Router model selected manually during setup. |
| ❌ No Cloud-Based Router Management | Only LAN-based control via ESP32. |

---

## Why These Non-Goals Matter

Each excluded feature reduces:
- **Firmware complexity** — smaller, more stable ESP32 code.
- **Testing surface** — fewer edge cases in MVP.
- **Security risk** — less attack surface on the device.
- **Time to market** — ship the core value proposition faster.

> **The MVP goal is:** Router Control Agent that manages profiles, quotas, schedules, and device blocking. Nothing more.
