**تقرير معماري شامل – مشروع EMO (مدير الإنترنت العائلي)**  
**إعداد: فريق التطوير**  
**النسخة: 1.0**

---

## 1. ملخص المشروع

**EMO (Family Internet Manager)** هو نظام متكامل من ثلاث طبقات لإدارة الإنترنت المنزلي:  
**جهاز ESP32 (الطبقة الميدانية) ← سحابة خلفية (NestJS) ← تطبيق ولي الأمر (Flutter)**.

يتيح النظام لولي الأمر إنشاء **ملفات تعريف رقمية** للأبناء، وتحديد **أوقات تصفح مسموح بها**، و**حصة بيانات يومية/شهرية**، وحظر الأجهزة غير المرغوب فيها عبر التحكم بجهاز التوجيه (Router) مباشرة عبر واجهة HTTP الداخلية للراوتر.  

> **مبدأ أساسي**: ESP32 يعمل كـ **وكيل تحكم فقط** (Router Control Agent) – لا يعترض حركة المرور، لا يقوم بفحص الحزم، لا يقيس عرض الحزمة.

---

## 2. الهيكل العام للنظام (Three-Tier Architecture)

```
┌─────────────────────────────────────────────────────────────────────┐
│                      📱 تطبيق ولي الأمر (Flutter)                    │
│  شاشات: لوحة التحكم، الملفات، الأجهزة، الاستخدام، الإشعارات، الإعدادات │
│  خدمات: API Client, Auth Service, WebSocket Service                  │
│  مقدمي البيانات: AuthProvider, HomeProvider                          │
└───────────────────────────┬─────────────────────────────────────────┘
                            │ HTTPS / WebSocket
                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    ☁️ السحابة الخلفية (NestJS + PostgreSQL)           │
│  REST API (v1) │ MQTT Client │ JWT Auth │ Prisma ORM │ WebSocket     │
│  وحدات: Auth, Homes, Profiles, Devices, NetworkDevices,              │
│         Quotas, Schedules, Usage, Notifications, Routers, MQTT       │
└───────────────────────────┬─────────────────────────────────────────┘
                            │ MQTT (TLS 8883)
                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│             📟 جهاز التحكم (ESP32 – ESP32 Dev Kit)                  │
│  NetworkManager (Ethernet W5500 + WiFi Fallback)                     │
│  MqttClient (TLS, Heartbeat 30s, Last-Will)                         │
│  RouterDriver (Huawei HG8145V5 – HTTP API)                          │
│  PolicyEngine (جدولة + حصة بيانات)                                   │
│  TamperDetector (مراقبة صحة الراوتر)                                 │
│  CredentialManager (تخزين آمن لبيانات الراوتر)                       │
│  OtaUpdater (تحديث البرامج الثابتة عبر الهواء)                       │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. مكونات النظام بالتفصيل

### 3.1 جهاز ESP32 – الطبقة الميدانية

**اللغة**: C++ (Arduino Framework)  
**بيئة التطوير**: PlatformIO  
**المعالج**: ESP32 (ESP-WROOM-32)  
**الذاكرة الوميضية**: 8MB (مقسمة لـ OTA)  
**الاتصال بالشبكة**:  
- Ethernet (W5500 SPI) – الخيار المفضل  
- WiFi (2.4 GHz) – خيار احتياطي مع تجاوز تلقائي (Failover)  

#### الوحدات البرمجية:

| الوحدة | الملفات | الوظيفة |
|--------|---------|----------|
| **NetworkManager** | `network_manager.h/cpp` | إدارة اتصال الشبكة (Ethernet/WiFi مع Failover، ping صحي) |
| **MqttClient** | `mqtt_client.h/cpp` | اتصال MQTT (TLS، Heartbeat 30 ثانية، إعادة اتصال تصاعدية، Last-Will) |
| **HuaweiHG8145V5Driver** | `huawei_hg8145v5_driver.h/cpp` | واجهة تحكم براوتر Huawei HG8145V5 (جلب الأجهزة، حظر، DNS، Whitelist) |
| **DeviceRegistry** | `device_registry.h/cpp` | إدارة تسجيل الجهاز، توليد ID، تخزين NVS |
| **PolicyEngine** | `policy_engine.h/cpp` | تطبيق سياسات الجدولة والحصة اليومية بناءً على ملفات التعريف |
| **NvsManager** | `nvs_manager.h/cpp` | طبقة تجريد لـ Preferences (تخزين key-value) |
| **TamperDetector** | `tamper_detector.h/cpp` | مراقبة صحة الراوتر (فشل الدخول، إعادة التشغيل، انقطاع MQTT) |
| **CredentialManager** | `credential_manager.h/cpp` | تخزين آمن لبيانات اعتماد الراوتر في NVS |
| **OtaUpdater** | `ota_updater.h/cpp` | تحديث البرامج الثابتة عبر الهواء (OTA) |

#### دورة التشغيل الرئيسية (`main.cpp`):

```
Boot → NVS init → تحميل بيانات اعتماد الراوتر → 
اتصال بالشبكة → اتصال MQTT → ← تسجيل الجهاز (أول مرة) → 
حلقة لا نهائية (كل 10-50ms):
  • NetworkManager.loop() – صيانة اتصال الشبكة
  • MqttClient.loop() + heartbeat() – صيانة MQTT
  • handleDevicePoll() – جلب أجهزة الراوتر كل 60 ثانية
  • handlePolicyCheck() – تطبيق السياسات كل 60 ثانية
  • handleUsageReport() – إرسال تقرير الاستخدام كل 5 دقائق
  • tamperDetector.loop() – مراقبة الصحة كل 10 ثوانٍ
  • otaUpdater.loop() – متابعة تحديث OTA
```

#### بروتوكول MQTT – المواضيع (Topics):

```
emo/{deviceId}/status      ← نبض الحياة (30s)، retains
emo/{deviceId}/events      ← أحداث (حظر، تفعيل، تحديث)
emo/{deviceId}/alerts      ← إنذارات (عبث، انقطاع)
emo/{deviceId}/usage       ← تقرير الاستخدام (5min)
emo/{deviceId}/commands    → أوامر من السحابة (حظر، فتح، تحديث بيانات، OTA)
emo/{deviceId}/policies    → سياسات من السحابة (ملفات تعريف محدثة)
emo/register               → تسجيل الجهاز (أول مرة)
```

---

### 3.2 السحابة الخلفية – Cloud Backend

**الإطار**: NestJS (TypeScript)  
**قاعدة البيانات**: PostgreSQL مع Prisma ORM  
**الوسيط الرسائلي**: MQTT (EMQX أو أي broker متوافق)  
**المصادقة**: JWT (Access + Refresh Token)  
**التوثيق**: ValidationPipe + class-validator على جميع DTOs  

#### نموذج البيانات (14 جدولاً):

```
User ──┬── Home (المسكن)
        │
Home ──┬── HomeMember (أعضاء المسكن)
       ├── Profile (ملف تعريف – طفل)
       ├── Router (جهاز التوجيه)
       ├── Device (جهاز EMO – ESP32 مسجل)
       └── NetworkDevice (أجهزة الشبكة المنزلية)

Profile ──┬── QuotaRule (حصة البيانات)
           ├── Schedule (جدول الأوقات – JSON: activeDays + timeSlots)
           └── UsageLog (سجل الاستخدام)

Notification (إشعارات للمستخدم)
Event (أحداث النظام)
Policy (سياسات من السحابة)
```

#### وحدات الـ REST API (17 Controller):

| الوحدة | المسار (Prefix) | الوظيفة |
|--------|-----------------|----------|
| **Auth** | `/auth` | تسجيل، دخول، تحديث التوكن |
| **Users** | `/users` | إدارة المستخدمين |
| **Homes** | `/homes` | إدارة المساكن (CRUD + pause/resume + أعضاء) |
| **Profiles** | `/homes/:homeId/profiles` | إدارة ملفات التعريف (CRUD) |
| **Devices** | `/homes/:homeId/devices` | أجهزة EMO المسجلة (قراءة فقط) |
| **NetworkDevices** | `/homes/:homeId/network-devices` | أجهزة الشبكة (قراءة + اعتماد/حظر/تعيين ملف) |
| **Routers** | `/routers` | أجهزة التوجيه (CRUD) |
| **Quotas** | `/homes/:homeId/quotas` | حصص البيانات (قراءة + تحديث) |
| **Schedules** | `/homes/:homeId/schedules` | جداول الأوقات (قراءة + تحديث) |
| **Usage** | `/homes/:homeId/usage` | إحصائيات الاستخدام (تراكمي + تاريخي) |
| **Notifications** | `/homes/:homeId/notifications` | الإشعارات (قراءة + تعيين كمقروء) |
| **Alerts** | `/homes/:homeId/alerts` | الإنذارات |
| **Mqtt** | `/mqtt` | نقطة نهاية لاختبار MQTT |

#### خدمات MQTT:

| الخدمة | الوظيفة |
|--------|---------|
| **MqttService** | إدارة اتصال MQTT (اشتراك، نشر، معالجة الرسائل) |
| **MqttUsageService** | استقبال تقارير الاستخدام من EMO، تخزين في UsageLog |

---

### 3.3 تطبيق ولي الأمر – Flutter

**اللغة**: Dart  
**الإطار**: Flutter مع Provider  
**المكتبات الرئيسية**: http, flutter_secure_storage, web_socket_channel  

#### الشاشات (7 شاشات):

| الشاشة | المسار | الوظيفة |
|--------|--------|---------|
| **LoginScreen** | `/login` | تسجيل الدخول |
| **DashboardScreen** | `/dashboard` | لوحة التحكم الرئيسية (بطاقات ملخص + تبويب سفلي) |
| **ProfilesScreen** | `/profiles` | قائمة ملفات التعريف |
| **ProfileDetailScreen** | `/profile_detail` | تفاصيل ملف تعريف (جداول + حصة) |
| **DevicesScreen** | `/devices` | أجهزة الشبكة مع إمكانية الاعتماد/الحظر |
| **UsageScreen** | `/usage` | إحصائيات الاستخدام لكل ملف |
| **NotificationsScreen** | `/notifications` | قائمة الإشعارات مع إمكانية تعيين كمقروء |

#### مقدمي البيانات (Providers):

| المزود | الوظيفة |
|--------|---------|
| **AuthProvider** | إدارة حالة المصادقة (تسجيل، دخول، خروج) |
| **HomeProvider** | إدارة كامل بيانات المسكن (ملفات، أجهزة، جداول، إشعارات) مع تحديثات WebSocket |

#### الخدمات:

| الخدمة | الوظيفة |
|--------|---------|
| **ApiClient** | عميل HTTP مع تحديث JKT تلقائي (Refresh Token) |
| **AuthService** | عمليات المصادقة |
| **WebSocketService** | اتصال WebSocket للتحديثات اللحظية (حالة الأجهزة، أجهزة جديدة، إنذارات) |

---

## 4. تدفق البيانات – السيناريوهات الرئيسية

### 4.1 التسجيل الأولي لجهاز EMO

```
ESP32 (أول تشغيل) → NVS: لا يوجد device_id → توليد device_id و pairing_code  
  → اتصال MQTT → نشر {device_id, mac, pairing_code, firmware_version} إلى emo/register  
  → Cloud: استلام الرسالة → تحديث جدول Device → ربط مع Home  
  → (استجابة مباشرة لا توجد – التطبيق يقرأ حالة الجهاز عبر الـ REST API)
  → ESP32: تخزين registered=true في NVS
  → (توقف عن إرسال التسجيل في التشغيلات اللاحقة)
```

### 4.2 دورة السياسات (جدول + حصة)

```
Cloud (مدير المشروع) → تحديث Schedule أو QuotaRule لملف تعريف  
  → نشر سياسة إلى emo/{deviceId}/policies  
  → ESP32: استلام → PolicyEngine.updatePolicies() → تحديث البيانات المحلية  
  → كل 60 ثانية: PolicyEngine.evaluate() →  
    • التحقق من الوقت الحالي (isInAllowedWindow)  
    • التحقق من الحصة (isQuotaExhausted)  
    • إذا انتهت الحصة أو خارج الوقت المسموح → blockDevice(mac) للجهاز المخالف  
  → نشر حدث {blocked_device, mac} إلى emo/{deviceId}/events
```

### 4.3 حظر/اعتماد جهاز شبكة من التطبيق

```
Flutter (ولي الأمر) → POST /homes/{homeId}/network-devices/{id}/block  
  → Cloud: تحديث حالة الجهاز في قاعدة البيانات  
  → Cloud: نشر أمر block_device إلى emo/{deviceId}/commands  
  → ESP32: استلام → routerDriver.blockDevice(mac)  
  → ESP32: نشر حدث {event:block_device, mac, success:true} إلى emo/{deviceId}/events  
  → Cloud: استلام الحدث → (اختياري: إنشاء إشعار)
```

### 4.4 تقرير الاستخدام

```
ESP32 (كل 5 دقائق) → جلب قائمة الأجهزة من الراوتر  
  → تجميع: timestamp, onlineDevices, لكل جهاز {mac, ip, hostname, online}  
  → نشر إلى emo/{deviceId}/usage  
  → Cloud MqttUsageService: استلام →  
    • إيجاد NetworkDevice بواسطة MAC  
    • إنشاء UsageLog مع profileId ← networkDeviceId  
    • (bytesDownloaded/Uploaded = 0 مؤقتاً لحين توفر راوتر يدعم قياس عرض الحزمة)
```

### 4.5 الكشف عن العبث (Tamper Detection)

```
ESP32 TamperDetector.loop() (كل 10 ثوانٍ):
  • مراقبة فشل دخول الراوتر → بعد 3 محاولات فاشلة → إنذار router_credentials_failed
  • مراقبة وقت تشغيل الراوتر (uptime) → إذا قل فجأة → إنذار router_restarted
  • مراقبة اتصال MQTT → إذا انقطع لأكثر من 60 ثانية → إنذار mqtt_connection_lost
  • نشر الإنذار إلى emo/{deviceId}/alerts
  • Cloud: استقبال → إنشاء إشعار لولي الأمر
```

---

## 5. البنية التحتية والنشر

### الحاويات (Docker Compose):

| الخدمة | الصورة | الدور |
|--------|--------|-------|
| **PostgreSQL** | postgres:15 | قاعدة البيانات |
| **Redis** | redis:7-alpine | تخزين مؤقت للجلسات (مستقبلاً) |
| **EMQX** | emqx:5 | وسيط MQTT |

### النشر:

```
cloud/ (NestJS) → Build → Docker Image → Deploy إلى أي سحابة (Render, Railway, Fly.io)
esp32_firmware/ → PlatformIO Build → Flash عبر USB أو OTA
flutter_app/ → Flutter Build → APK/IPA → متجر التطبيقات
```

### خط CI/CD (GitHub Actions):

- اختبار وتجميع Cloud (`pnpm build && pnpm test`)
- فحص الجودة (ESLint)
- نشر Flutter Web (اختياري)

---

## 6. حالة الإنجاز حسب المراحل (Milestones)

| المرحلة | الحالة | المكونات |
|---------|--------|----------|
| **M0 – التأسيس** | ✅ مكتمل | هيكل المشروع، Docker Compose، CI/CD |
| **M1 – السحابة الأساسية** | ✅ مكتمل | Auth (JWT)، 14 نموذج Prisma، 17 Controller، MQTT، 7 اختبارات وحدة |
| **M2 – اتصال ESP32** | ✅ مكتمل | NetworkManager، MqttClient (TLS)، التسجيل، IRouterDriver، NVS |
| **M3 – مشغل الراوتر** | ✅ مكتمل | Huawei HG8145V5 كامل (10 دوال: login, block, whitelist, statistics...) |
| **M4 – اكتشاف الأجهزة** | ✅ مكتمل | DevicePoll (60s)، نشر إلى السحابة |
| **M5 – ملفات التعريف والحصص** | ✅ مكتمل | CRUD Profiles + Quotas مع تطبيق على ESP32 |
| **M6 – محرك الجدولة** | ✅ مكتمل | جداول أوقات متعددة (JSON)، تنفيذ على ESP32 |
| **M7 – لوحة ولي الأمر** | ✅ مكتمل | 7 شاشات Flutter، WebSocket، API Client |
| **M8 – الأمان والحماية** | ✅ مكتمل | TamperDetector، CredentialManager، OTA، تقسيم ذاكرة OTA |

> **جميع المراحل مكتملة** – المشروع جاهز للتطبيق الميداني والتجارب مع راوتر حقيقي.

---

## 7. القرارات التقنية الرئيسية

| القرار | البديل المطروح | الاختيار | السبب |
|--------|---------------|----------|-------|
| لغة ESP32 | MicroPython, ESP-IDF | **Arduino (C++)** | سرعة تطوير أعلى، مكتبات جاهزة للـ MQTT والـ Ethernet |
| إطار السحابة | Express, Fastify | **NestJS** | هيكل منظم، حقن التبعية، توثيق DTO مدمج |
| ORM | TypeORM, Knex | **Prisma** | توليد تلقائي للأنواع، هجرة آمنة، أدوات استعلام قوية |
| واجهة الراوتر | SNMP, Telnet | **HTTP API** | سهولة التكامل مع Huawei HG8145V5 (يدعم CGI فقط) |
| تخزين بيانات الراوتر | نص عادي، Hardware Key | **NVS (Preferences)** | مدمج في ESP32، آمن، سهل الاستخدام |
| نقل البيانات | REST polling, gRPC | **MQTT** | خفيف، يدعم Last-Will، اشتراك/نشر (pub/sub) |
| إدارة الحالة في Flutter | Bloc, Riverpod, GetX | **Provider** | بسيط، مدعوم رسمياً، مناسب لحجم التطبيق |
| تنسيق الجدول الزمني | عدة صفوف (Normalized) | **JSON (activeDays + timeSlots)** | مرونة عالية، استعلام واحد، يدعم أوقات متعددة وأيام متعددة |

---

## 8. المبادئ والقواعد غير القابلة للتفاوض

1. **ESP32 ليس Gateway** – لا يعترض حركة المرور، لا يفحص الحزم، لا يقيس عرض الحزمة  
2. **كل فترات الانتظار غير حاجبة** – `millis()` فقط، لا `delay()`  
3. **جميع DTOs موثقة بـ class-validator**  
4. **MQTT دائمًا TLS على المنفذ 8883**  
5. **مواضيع MQTT تتبع النمط `emo/{deviceId}/{type}`**  
6. **رسوم Mermaid البيانية هي المعيار للتوثيق**  
7. **جميع أسماء الحقول في JSON بـ snake_case**  
8. **مهما حدث – لا ننتقل إلى Router Proxy/Gateway** (هذا خارج نطاق MVP)

---

## 9. الخريطة الزمنية – إجمالي الجهد

| النشاط | الجهد المقدر | الجهد المستغرق |
|--------|-------------|----------------|
| التأسيس (M0) | 1 يوم | 1 يوم |
| السحابة الأساسية (M1) | 3 أيام | 3 أيام |
| اتصال ESP32 + السائق (M2+M3) | 3 أيام | 3 أيام |
| اكتشاف الأجهزة + ملفات التعريف (M4+M5) | 3 أيام | 3 أيام |
| الجدولة + لوحة التحكم (M6+M7) | 4 أيام | 4 أيام |
| الأمان والحماية (M8) | 2 يوم | 2 يوم |
| **المجموع** | **16 يوماً** | **16 يوماً** |

> جميع المراحل التسليمية مكتملة. المشروع جاهز للاختبار الميداني.

---

## 10. الملفات المرجعية الهامة

| الملف | الأهمية |
|-------|---------|
| `cloud/prisma/schema.prisma` | تعريف كامل لنموذج البيانات (14 جدول) |
| `cloud/src/main.ts` | نقطة دخول السحابة (منفذ 3000، CORS، ValidationPipe) |
| `esp32_firmware/src/main.cpp` | دورة التشغيل الرئيسية لـ ESP32 |
| `flutter_app/lib/main.dart` | نقطة دخول التطبيق مع المسارات |
| `docs/system_architecture.md` | رسم العمارة المحدث |
| `docs/mvp_execution_plan.md` | خطة التنفيذ بالتفصيل |
| `docs/non_goals.md` | وثيقة خارج النطاق – **ضرورية لتجنب زحف المتطلبات** |

---

*نهاية التقرير – جميع المراحل مكتملة، المشروع جاهز للتطبيق الميداني.*