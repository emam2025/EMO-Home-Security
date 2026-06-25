import { Injectable, NotFoundException, ForbiddenException } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';
import { CreateHomeDto } from './dto/create-home.dto';
import { UpdateHomeDto } from './dto/update-home.dto';

@Injectable()
export class HomesService {
  constructor(private prisma: PrismaService) {}

  async create(ownerId: string, dto: CreateHomeDto) {
    const home = await this.prisma.home.create({
      data: {
        name: dto.name,
        timezone: dto.timezone ?? 'UTC',
        ownerId,
      },
    });
    await this.prisma.homeMember.create({
      data: {
        homeId: home.id,
        userId: ownerId,
        role: 'admin',
      },
    });
    return home;
  }

  async findAll(userId: string) {
    return this.prisma.home.findMany({
      where: { members: { some: { userId } } },
      include: { members: true, profiles: true, routers: true },
    });
  }

  async findOne(id: string, userId?: string) {
    const home = await this.prisma.home.findUnique({
      where: { id },
      include: { members: true, profiles: true, routers: true },
    });
    if (!home) throw new NotFoundException('Home not found');
    if (userId) {
      const isMember = home.members.some((m) => m.userId === userId);
      if (!isMember) throw new ForbiddenException('Access denied');
    }
    return home;
  }

  async update(id: string, dto: UpdateHomeDto, userId?: string) {
    await this.findOne(id, userId);
    return this.prisma.home.update({ where: { id }, data: dto });
  }

  async remove(id: string, userId?: string) {
    await this.findOne(id, userId);
    return this.prisma.home.delete({ where: { id } });
  }

  async addMember(homeId: string, adminUserId: string, memberUserId: string, role: string) {
    const home = await this.findOne(homeId, adminUserId);
    const isAdmin = home.members.some((m) => m.userId === adminUserId && m.role === 'admin');
    if (!isAdmin) throw new ForbiddenException('Only admins can add members');

    return this.prisma.homeMember.create({
      data: { homeId, userId: memberUserId, role: role as any },
    });
  }
}
