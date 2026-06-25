import { Injectable } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';
import { MqttService } from '../mqtt/mqtt.service';

@Injectable()
export class MqttUsageService {
  constructor(
    private prisma: PrismaService,
    mqttService: MqttService,
  ) {
    mqttService.onUsage((deviceId, payload) => this.handleUsageReport(deviceId, payload));
  }

  async handleUsageReport(deviceId: string, payload: string) {
    try {
      const report = JSON.parse(payload) as {
        timestamp: number;
        uptime: number;
        total_devices: number;
        online_devices: number;
        devices: { mac: string; ip: string; hostname: string; online: boolean }[];
      };

      const device = await this.prisma.device.findUnique({ where: { id: deviceId } });
      if (!device) return;

      for (const dev of report.devices) {
        const networkDevice = await this.prisma.networkDevice.findFirst({
          where: { homeId: device.homeId, macAddress: dev.mac },
        });
        if (!networkDevice) continue;

        const profileId = networkDevice.profileId;
        if (!profileId) continue;

        await this.prisma.usageLog.create({
          data: {
            profileId,
            networkDeviceId: networkDevice.id,
            bytesDownloaded: BigInt(0),
            bytesUploaded: BigInt(0),
            loggedAt: new Date(report.timestamp * 1000),
          },
        });
      }
    } catch (error) {
      console.warn(
        'Ignoring malformed usage report:',
        error instanceof Error ? error.message : String(error),
      );
    }
  }
}
