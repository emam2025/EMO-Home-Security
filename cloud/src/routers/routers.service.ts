import { Injectable, NotFoundException } from '@nestjs/common';
import { Prisma } from '@prisma/client';
import { PrismaService } from '../prisma/prisma.service';

@Injectable()
export class RoutersService {
  constructor(private prisma: PrismaService) {}

  async create(data: {
    homeId: string;
    model: string;
    manufacturer: string;
    ipAddress: string;
    macAddress: string;
    serialNumber?: string;
    credentialsEncrypted?: string;
    factoryCredentialsEncrypted?: string;
  }) {
    return this.prisma.router.create({ data: data as Prisma.RouterUncheckedCreateInput });
  }

  async findByHome(homeId: string) {
    return this.prisma.router.findMany({ where: { homeId } });
  }

  async findOne(id: string) {
    const router = await this.prisma.router.findUnique({ where: { id } });
    if (!router) throw new NotFoundException('Router not found');
    return router;
  }

  async update(
    id: string,
    data: {
      ipAddress?: string;
      firmwareVersion?: string;
      isManaged?: boolean;
      configSnapshot?: Prisma.InputJsonValue;
    },
  ) {
    const router = await this.prisma.router.findUnique({ where: { id } });
    if (!router) throw new NotFoundException('Router not found');
    return this.prisma.router.update({
      where: { id },
      data: data as Prisma.RouterUncheckedUpdateInput,
    });
  }

  async remove(id: string) {
    const router = await this.prisma.router.findUnique({ where: { id } });
    if (!router) throw new NotFoundException('Router not found');
    return this.prisma.router.delete({ where: { id } });
  }
}
