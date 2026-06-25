import { Injectable, NotFoundException } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';

@Injectable()
export class ProfilesService {
  constructor(private prisma: PrismaService) {}

  async create(homeId: string, data: { name: string; avatar?: string }) {
    return this.prisma.profile.create({
      data: { homeId, ...data },
    });
  }

  async findByHome(homeId: string) {
    return this.prisma.profile.findMany({ where: { homeId } });
  }

  async findOne(id: string) {
    const profile = await this.prisma.profile.findUnique({ where: { id } });
    if (!profile) throw new NotFoundException('Profile not found');
    return profile;
  }

  async update(id: string, data: { name?: string; avatar?: string; isActive?: boolean }) {
    const profile = await this.prisma.profile.findUnique({ where: { id } });
    if (!profile) throw new NotFoundException('Profile not found');
    return this.prisma.profile.update({ where: { id }, data });
  }

  async remove(id: string) {
    const profile = await this.prisma.profile.findUnique({ where: { id } });
    if (!profile) throw new NotFoundException('Profile not found');
    return this.prisma.profile.delete({ where: { id } });
  }
}
