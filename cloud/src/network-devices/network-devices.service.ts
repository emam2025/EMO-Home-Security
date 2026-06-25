import { Injectable, NotFoundException } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';
import { NetworkDeviceStatus } from '@prisma/client';

@Injectable()
export class NetworkDevicesService {
  constructor(private prisma: PrismaService) {}

  async create(data: {
    homeId: string;
    routerId: string;
    profileId?: string;
    macAddress: string;
    ipAddress?: string;
    hostname?: string;
    vendor?: string;
    status?: NetworkDeviceStatus;
  }) {
    return this.prisma.networkDevice.create({ data });
  }

  async findByHome(homeId: string) {
    return this.prisma.networkDevice.findMany({ where: { homeId } });
  }

  async findOne(id: string) {
    const device = await this.prisma.networkDevice.findUnique({ where: { id } });
    if (!device) throw new NotFoundException('Network device not found');
    return device;
  }

  async update(
    id: string,
    data: {
      profileId?: string;
      ipAddress?: string;
      hostname?: string;
      vendor?: string;
      status?: NetworkDeviceStatus;
    },
  ) {
    const device = await this.prisma.networkDevice.findUnique({ where: { id } });
    if (!device) throw new NotFoundException('Network device not found');
    return this.prisma.networkDevice.update({ where: { id }, data });
  }

  async remove(id: string) {
    const device = await this.prisma.networkDevice.findUnique({ where: { id } });
    if (!device) throw new NotFoundException('Network device not found');
    return this.prisma.networkDevice.delete({ where: { id } });
  }
}
