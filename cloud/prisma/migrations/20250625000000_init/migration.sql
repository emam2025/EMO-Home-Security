-- CreateEnum
CREATE TYPE "UserRole" AS ENUM ('parent', 'admin');

-- CreateEnum
CREATE TYPE "HomeMemberRole" AS ENUM ('parent', 'child', 'admin');

-- CreateEnum
CREATE TYPE "NetworkDeviceStatus" AS ENUM ('pending', 'approved', 'blocked', 'unknown');

-- CreateEnum
CREATE TYPE "ActionOnExhaust" AS ENUM ('block', 'throttle');

-- CreateEnum
CREATE TYPE "DnsProvider" AS ENUM ('cloudflare', 'google', 'opendns', 'custom');

-- CreateEnum
CREATE TYPE "NewDeviceDefaultAction" AS ENUM ('approve', 'block', 'notify');

-- CreateTable
CREATE TABLE "User" (
    "id" TEXT NOT NULL,
    "email" TEXT NOT NULL,
    "passwordHash" TEXT NOT NULL,
    "name" TEXT NOT NULL,
    "role" "UserRole" NOT NULL DEFAULT 'parent',
    "fcmToken" TEXT,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "User_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Home" (
    "id" TEXT NOT NULL,
    "name" TEXT NOT NULL,
    "ownerId" TEXT NOT NULL,
    "timezone" TEXT NOT NULL DEFAULT 'UTC',
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "Home_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "HomeMember" (
    "homeId" TEXT NOT NULL,
    "userId" TEXT NOT NULL,
    "role" "HomeMemberRole" NOT NULL DEFAULT 'child',

    CONSTRAINT "HomeMember_pkey" PRIMARY KEY ("homeId", "userId")
);

-- CreateTable
CREATE TABLE "Profile" (
    "id" TEXT NOT NULL,
    "homeId" TEXT NOT NULL,
    "name" TEXT NOT NULL,
    "avatar" TEXT,
    "isActive" BOOLEAN NOT NULL DEFAULT true,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "Profile_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Router" (
    "id" TEXT NOT NULL,
    "homeId" TEXT NOT NULL,
    "model" TEXT NOT NULL,
    "manufacturer" TEXT NOT NULL,
    "ipAddress" TEXT NOT NULL,
    "macAddress" TEXT NOT NULL,
    "serialNumber" TEXT,
    "firmwareVersion" TEXT,
    "credentialsEncrypted" TEXT,
    "factoryCredentialsEncrypted" TEXT,
    "configSnapshot" JSONB,
    "fingerprint" TEXT,
    "isManaged" BOOLEAN NOT NULL DEFAULT false,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "Router_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Device" (
    "id" TEXT NOT NULL,
    "homeId" TEXT NOT NULL,
    "routerId" TEXT NOT NULL,
    "macAddress" TEXT NOT NULL,
    "firmwareVersion" TEXT,
    "mqttUsername" TEXT,
    "mqttPasswordEncrypted" TEXT,
    "pairingCode" TEXT,
    "pairedAt" TIMESTAMPTZ,
    "lastSeenAt" TIMESTAMPTZ,
    "isOnline" BOOLEAN NOT NULL DEFAULT false,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "Device_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "NetworkDevice" (
    "id" TEXT NOT NULL,
    "homeId" TEXT NOT NULL,
    "routerId" TEXT NOT NULL,
    "profileId" TEXT,
    "macAddress" TEXT NOT NULL,
    "ipAddress" TEXT,
    "hostname" TEXT,
    "vendor" TEXT,
    "status" "NetworkDeviceStatus" NOT NULL DEFAULT 'pending',
    "firstSeenAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "lastSeenAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "NetworkDevice_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "QuotaRule" (
    "id" TEXT NOT NULL,
    "profileId" TEXT NOT NULL,
    "quotaGb" DECIMAL(10,2) NOT NULL DEFAULT 0,
    "consumedGb" DECIMAL(10,2) NOT NULL DEFAULT 0,
    "period" TEXT NOT NULL DEFAULT 'monthly',
    "actionOnExhaust" "ActionOnExhaust" NOT NULL DEFAULT 'block',
    "resetDay" INTEGER NOT NULL DEFAULT 1,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "QuotaRule_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Schedule" (
    "id" TEXT NOT NULL,
    "profileId" TEXT NOT NULL,
    "enabled" BOOLEAN NOT NULL DEFAULT true,
    "activeDays" JSONB NOT NULL DEFAULT '[]',
    "timeSlots" JSONB NOT NULL DEFAULT '[]',
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "Schedule_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "UsageLog" (
    "id" TEXT NOT NULL,
    "profileId" TEXT NOT NULL,
    "networkDeviceId" TEXT NOT NULL,
    "bytesDownloaded" BIGINT NOT NULL DEFAULT 0,
    "bytesUploaded" BIGINT NOT NULL DEFAULT 0,
    "loggedAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "UsageLog_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Policy" (
    "id" TEXT NOT NULL,
    "homeId" TEXT NOT NULL,
    "dnsProvider" "DnsProvider" NOT NULL DEFAULT 'cloudflare',
    "customDnsPrimary" TEXT,
    "customDnsSecondary" TEXT,
    "whitelistEnabled" BOOLEAN NOT NULL DEFAULT false,
    "newDeviceDefaultAction" "NewDeviceDefaultAction" NOT NULL DEFAULT 'notify',
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "Policy_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Notification" (
    "id" TEXT NOT NULL,
    "userId" TEXT NOT NULL,
    "type" TEXT NOT NULL,
    "title" TEXT NOT NULL,
    "body" TEXT,
    "data" JSONB,
    "isRead" BOOLEAN NOT NULL DEFAULT false,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "Notification_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Event" (
    "id" TEXT NOT NULL,
    "deviceId" TEXT NOT NULL,
    "type" TEXT NOT NULL,
    "payload" JSONB,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "Event_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "User_email_key" ON "User"("email");

-- CreateIndex
CREATE INDEX "User_email_idx" ON "User"("email");

-- CreateIndex
CREATE INDEX "Home_ownerId_idx" ON "Home"("ownerId");

-- CreateIndex
CREATE INDEX "HomeMember_userId_idx" ON "HomeMember"("userId");

-- CreateIndex
CREATE INDEX "Profile_homeId_idx" ON "Profile"("homeId");

-- CreateIndex
CREATE INDEX "Router_homeId_idx" ON "Router"("homeId");

-- CreateIndex
CREATE INDEX "Router_macAddress_idx" ON "Router"("macAddress");

-- CreateIndex
CREATE INDEX "Router_serialNumber_idx" ON "Router"("serialNumber");

-- CreateIndex
CREATE UNIQUE INDEX "Device_macAddress_key" ON "Device"("macAddress");

-- CreateIndex
CREATE INDEX "Device_homeId_idx" ON "Device"("homeId");

-- CreateIndex
CREATE INDEX "Device_routerId_idx" ON "Device"("routerId");

-- CreateIndex
CREATE INDEX "Device_macAddress_idx" ON "Device"("macAddress");

-- CreateIndex
CREATE INDEX "NetworkDevice_homeId_idx" ON "NetworkDevice"("homeId");

-- CreateIndex
CREATE INDEX "NetworkDevice_routerId_idx" ON "NetworkDevice"("routerId");

-- CreateIndex
CREATE INDEX "NetworkDevice_profileId_idx" ON "NetworkDevice"("profileId");

-- CreateIndex
CREATE INDEX "NetworkDevice_macAddress_idx" ON "NetworkDevice"("macAddress");

-- CreateIndex
CREATE INDEX "NetworkDevice_status_idx" ON "NetworkDevice"("status");

-- CreateIndex
CREATE INDEX "QuotaRule_profileId_idx" ON "QuotaRule"("profileId");

-- CreateIndex
CREATE INDEX "Schedule_profileId_idx" ON "Schedule"("profileId");

-- CreateIndex
CREATE INDEX "UsageLog_profileId_idx" ON "UsageLog"("profileId");

-- CreateIndex
CREATE INDEX "UsageLog_networkDeviceId_idx" ON "UsageLog"("networkDeviceId");

-- CreateIndex
CREATE INDEX "UsageLog_loggedAt_idx" ON "UsageLog"("loggedAt");

-- CreateIndex
CREATE INDEX "Policy_homeId_idx" ON "Policy"("homeId");

-- CreateIndex
CREATE INDEX "Notification_userId_idx" ON "Notification"("userId");

-- CreateIndex
CREATE INDEX "Notification_isRead_idx" ON "Notification"("isRead");

-- CreateIndex
CREATE INDEX "Event_deviceId_idx" ON "Event"("deviceId");

-- CreateIndex
CREATE INDEX "Event_type_idx" ON "Event"("type");

-- CreateIndex
CREATE INDEX "Event_createdAt_idx" ON "Event"("createdAt");

-- AddForeignKey
ALTER TABLE "Home" ADD CONSTRAINT "Home_ownerId_fkey" FOREIGN KEY ("ownerId") REFERENCES "User"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "HomeMember" ADD CONSTRAINT "HomeMember_homeId_fkey" FOREIGN KEY ("homeId") REFERENCES "Home"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "HomeMember" ADD CONSTRAINT "HomeMember_userId_fkey" FOREIGN KEY ("userId") REFERENCES "User"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Profile" ADD CONSTRAINT "Profile_homeId_fkey" FOREIGN KEY ("homeId") REFERENCES "Home"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Router" ADD CONSTRAINT "Router_homeId_fkey" FOREIGN KEY ("homeId") REFERENCES "Home"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Device" ADD CONSTRAINT "Device_homeId_fkey" FOREIGN KEY ("homeId") REFERENCES "Home"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Device" ADD CONSTRAINT "Device_routerId_fkey" FOREIGN KEY ("routerId") REFERENCES "Router"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "NetworkDevice" ADD CONSTRAINT "NetworkDevice_homeId_fkey" FOREIGN KEY ("homeId") REFERENCES "Home"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "NetworkDevice" ADD CONSTRAINT "NetworkDevice_routerId_fkey" FOREIGN KEY ("routerId") REFERENCES "Router"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "NetworkDevice" ADD CONSTRAINT "NetworkDevice_profileId_fkey" FOREIGN KEY ("profileId") REFERENCES "Profile"("id") ON DELETE SET NULL ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "QuotaRule" ADD CONSTRAINT "QuotaRule_profileId_fkey" FOREIGN KEY ("profileId") REFERENCES "Profile"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Schedule" ADD CONSTRAINT "Schedule_profileId_fkey" FOREIGN KEY ("profileId") REFERENCES "Profile"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "UsageLog" ADD CONSTRAINT "UsageLog_profileId_fkey" FOREIGN KEY ("profileId") REFERENCES "Profile"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "UsageLog" ADD CONSTRAINT "UsageLog_networkDeviceId_fkey" FOREIGN KEY ("networkDeviceId") REFERENCES "NetworkDevice"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Policy" ADD CONSTRAINT "Policy_homeId_fkey" FOREIGN KEY ("homeId") REFERENCES "Home"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Notification" ADD CONSTRAINT "Notification_userId_fkey" FOREIGN KEY ("userId") REFERENCES "User"("id") ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Event" ADD CONSTRAINT "Event_deviceId_fkey" FOREIGN KEY ("deviceId") REFERENCES "Device"("id") ON DELETE CASCADE ON UPDATE CASCADE;
