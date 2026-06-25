# خطة تنفيذ MVP — 8 أسابيع

> EMO Family Internet Manager
> Realistic plan to reach a home-testable MVP

---

## Milestone 0 — Project Foundation (3–4 أيام)

**الهدف:** تجهيز بيئة التطوير والبنية الأساسية.

| المهمة | التفاصيل |
|---|---|
| **Repository Setup** | إنشاء هيكل المشروع: `Firmware/`, `Backend/`, `Mobile/`, `Docs/` |
| **Infrastructure** | GitHub repo, Docker Compose (PostgreSQL + EMQX + Redis), CI/CD via GitHub Actions (lint, unit tests, build verification) |

**الناتج:**
- ✅ مشروع منظم
- ✅ بيئات التطوير جاهزة

---

## Milestone 1 — Cloud Core (الأسبوع الأول)

**الهدف:** بناء Backend أساسي.

### Backend
| المهمة | النقاط الفرعية |
|---|---|
| **Authentication** | Register, Login, JWT, Refresh Token |
| **Database** | الجداول الأساسية: `users`, `homes`, `profiles`, `devices`, `routers` |
| **MQTT** | إعداد EMQX, TLS, تعريف Topics |

**الناتج:**
- ✅ مستخدم يستطيع إنشاء حساب
- ✅ قاعدة بيانات جاهزة
- ✅ MQTT Broker يعمل

---

## Milestone 2 — ESP32 Connectivity (الأسبوع الثاني)

**الهدف:** ربط ESP32 بالسحابة.

### Firmware
| المهمة | التفاصيل |
|---|---|
| **WiFi / Ethernet** | Connect, Reconnect, Heartbeat |
| **MQTT** | Publish, Subscribe, TLS |
| **Device Registration** | أول تشغيل: ESP32 → Register Device → Cloud |

### Topics
| Topic | Direction |
|---|---|
| `emo/{deviceId}/status` | Device → Cloud |
| `emo/{deviceId}/events` | Device → Cloud |
| `emo/{deviceId}/commands` | Cloud → Device |

**الناتج:**
- ✅ ESP32 يظهر Online
- ✅ Heartbeat كل 30 ثانية

---

## Milestone 3 — Router Driver V1 (الأسبوع الثالث)

**الهدف:** دعم أول راوتر فقط.

### الراوتر المستهدف
Huawei HG8145V5

### بناء
| المكون | الوظائف |
|---|---|
| `IRouterDriver` | الواجهة الأساسية |
| التنفيذ | `login()`, `getDevices()`, `blockDevice()`, `unblockDevice()`, `setDNS()` |

### اختبار
- Read Device List
- Block Device
- Unblock Device

**الناتج:**
- ✅ تحكم فعلي بالراوتر

---

## Milestone 4 — Device Discovery (الأسبوع الرابع)

**الهدف:** اكتشاف الأجهزة وإدارتها.

### Firmware
- Polling: كل 60 ثانية

### Backend
- Endpoints: `GET /devices`, `POST /assign-profile`

### App
- شاشة: Connected Devices

**الناتج:**
- ✅ رؤية الأجهزة المتصلة
- ✅ ربط الجهاز بمستخدم

---

## Milestone 5 — User Profiles & Quotas (الأسبوع الخامس)

**الهدف:** تطبيق نظام المستخدمين.

| المجال | العناصر |
|---|---|
| **Profiles** | Father, Mother, Ahmed, Mariam |
| **Policies** | Monthly Quota لكل ملف |
| **Backend** | `quota_rules`, `usage_logs` |
| **App** | Profile Screen |

**الناتج:**
- ✅ إنشاء ملفات شخصية
- ✅ ربط أجهزة بكل مستخدم

---

## Milestone 6 — Scheduling Engine (الأسبوع السادس)

**الهدف:** التحكم الزمني.

### Rules
مثال: Ahmed — 4 PM → 10 PM

### Firmware
- كل دقيقة: Check Schedule → Block / Unblock

**الناتج:**
- ✅ الإنترنت يعمل ضمن أوقات محددة

---

## Milestone 7 — Parent Dashboard (الأسبوع السابع)

**الهدف:** واجهة تشغيل حقيقية.

### Flutter
| الشاشة | العناصر |
|---|---|
| **Dashboard** | نظرة عامة |
| **Devices** | قائمة الأجهزة |
| **Profiles** | إدارة الملفات الشخصية |
| **Usage** | الاستهلاك |
| **Status** | حالة الجهاز والراوتر |

### Actions
- Pause Internet
- Resume Internet
- Add Time
- Add Quota

### Notifications
- New Device
- Offline Device
- Quota Exhausted

**الناتج:**
- ✅ تطبيق قابل للاستخدام

---

## Milestone 8 — Security & Hardening (الأسبوع الثامن)

**الهدف:** تثبيت النظام للاختبار المنزلي.

| المجال | التفاصيل |
|---|---|
| **Tamper Detection** | اكتشاف: Router Restart, Router Reset, EMO Offline |
| **Recovery** | Router Fingerprint, Credential Recovery, Policy Reapply |
| **OTA** | Firmware Update |

**الناتج:**
- ✅ MVP مستقر
- ✅ جاهز للاختبار المنزلي

---

## ما يتم تأجيله بعد MVP

### Phase 2
- ❌ تعدد الراوترات
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

## تعريف النجاح الحقيقي للـ MVP

> إذا استطعت تنفيذ السيناريو التالي بالكامل:

1. الأب يفتح التطبيق
2. يرى الأجهزة المتصلة
3. ينشئ "أحمد"
4. يربط جهاز أحمد
5. يحدد: 30 GB, 4 PM → 10 PM
6. يضغط Save
7. EMO يطبق السياسة على الراوتر
8. عند 10 PM يتم إيقاف الإنترنت تلقائيًا
9. الأب يرى الحالة من خارج المنزل

**فقد حققت MVP ناجحًا وقابلًا للتجربة مع المستخدمين الحقيقيين قبل أي توسعات أخرى.**
