import { Injectable, NotFoundException } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';

@Injectable()
export class DevicesService {
  constructor(private prisma: PrismaService) {}

  async create(data: {
    homeId: string;
    routerId: string;
    macAddress: string;
    firmwareVersion?: string;
    mqttUsername?: string;
    mqttPasswordEncrypted?: string;
    pairingCode?: string;
  }) {
    return this.prisma.device.create({ data });
  }

  async findByHome(homeId: string) {
    return this.prisma.device.findMany({ where: { homeId } });
  }

  async findByRouter(routerId: string) {
    return this.prisma.device.findMany({ where: { routerId } });
  }

  async findOne(id: string) {
    const device = await this.prisma.device.findUnique({ where: { id } });
    if (!device) throw new NotFoundException('Device not found');
    return device;
  }

  async update(
    id: string,
    data: { firmwareVersion?: string; isOnline?: boolean; lastSeenAt?: Date },
  ) {
    const device = await this.prisma.device.findUnique({ where: { id } });
    if (!device) throw new NotFoundException('Device not found');
    return this.prisma.device.update({ where: { id }, data });
  }

  async remove(id: string) {
    const device = await this.prisma.device.findUnique({ where: { id } });
    if (!device) throw new NotFoundException('Device not found');
    return this.prisma.device.delete({ where: { id } });
  }
}
