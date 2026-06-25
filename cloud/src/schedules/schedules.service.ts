import { Injectable, NotFoundException } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';

@Injectable()
export class SchedulesService {
  constructor(private prisma: PrismaService) {}

  async create(data: {
    profileId: string;
    enabled?: boolean;
    activeDays?: number[];
    timeSlots?: { startHour: number; startMinute: number; endHour: number; endMinute: number }[];
  }) {
    return this.prisma.schedule.create({
      data: {
        profileId: data.profileId,
        enabled: data.enabled ?? true,
        activeDays: data.activeDays ?? [],
        timeSlots: data.timeSlots ?? [],
      },
    });
  }

  async findByProfile(profileId: string) {
    return this.prisma.schedule.findMany({ where: { profileId } });
  }

  async findByHome(homeId: string) {
    const profiles = await this.prisma.profile.findMany({
      where: { homeId },
      select: { id: true },
    });
    const profileIds = profiles.map((p) => p.id);
    return this.prisma.schedule.findMany({ where: { profileId: { in: profileIds } } });
  }

  async findOne(id: string) {
    const schedule = await this.prisma.schedule.findUnique({ where: { id } });
    if (!schedule) throw new NotFoundException('Schedule not found');
    return schedule;
  }

  async update(
    id: string,
    data: {
      enabled?: boolean;
      activeDays?: number[];
      timeSlots?: { startHour: number; startMinute: number; endHour: number; endMinute: number }[];
    },
  ) {
    const schedule = await this.prisma.schedule.findUnique({ where: { id } });
    if (!schedule) throw new NotFoundException('Schedule not found');
    return this.prisma.schedule.update({ where: { id }, data });
  }

  async remove(id: string) {
    const schedule = await this.prisma.schedule.findUnique({ where: { id } });
    if (!schedule) throw new NotFoundException('Schedule not found');
    return this.prisma.schedule.delete({ where: { id } });
  }

  async updateForProfile(
    profileId: string,
    data: {
      enabled?: boolean;
      activeDays?: number[];
      timeSlots?: { startHour: number; startMinute: number; endHour: number; endMinute: number }[];
    },
  ) {
    const existing = await this.prisma.schedule.findFirst({ where: { profileId } });
    if (existing) {
      return this.prisma.schedule.update({ where: { id: existing.id }, data });
    }
    return this.prisma.schedule.create({
      data: { ...data, profileId, enabled: data.enabled ?? true },
    });
  }
}
