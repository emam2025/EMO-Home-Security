import { Injectable, NotFoundException } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';
import { ActionOnExhaust } from '@prisma/client';

@Injectable()
export class QuotasService {
  constructor(private prisma: PrismaService) {}

  async findByHome(homeId: string) {
    const profiles = await this.prisma.profile.findMany({
      where: { homeId },
      select: { id: true },
    });
    const profileIds = profiles.map((p) => p.id);
    return this.prisma.quotaRule.findMany({ where: { profileId: { in: profileIds } } });
  }

  async updateForProfile(profileId: string, data: { quotaGb?: number; period?: string }) {
    const existing = await this.prisma.quotaRule.findFirst({ where: { profileId } });
    if (existing) {
      return this.prisma.quotaRule.update({ where: { id: existing.id }, data });
    }
    return this.prisma.quotaRule.create({
      data: { ...data, profileId, period: data.period ?? 'monthly' },
    } as any);
  }

  async create(data: {
    profileId: string;
    quotaGb: number;
    period?: string;
    actionOnExhaust?: ActionOnExhaust;
    resetDay?: number;
  }) {
    return this.prisma.quotaRule.create({ data });
  }

  async findByProfile(profileId: string) {
    return this.prisma.quotaRule.findMany({ where: { profileId } });
  }

  async findOne(id: string) {
    const rule = await this.prisma.quotaRule.findUnique({ where: { id } });
    if (!rule) throw new NotFoundException('Quota rule not found');
    return rule;
  }

  async update(
    id: string,
    data: {
      quotaGb?: number;
      consumedGb?: number;
      period?: string;
      actionOnExhaust?: ActionOnExhaust;
      resetDay?: number;
    },
  ) {
    const rule = await this.prisma.quotaRule.findUnique({ where: { id } });
    if (!rule) throw new NotFoundException('Quota rule not found');
    return this.prisma.quotaRule.update({ where: { id }, data });
  }

  async remove(id: string) {
    const rule = await this.prisma.quotaRule.findUnique({ where: { id } });
    if (!rule) throw new NotFoundException('Quota rule not found');
    return this.prisma.quotaRule.delete({ where: { id } });
  }

  async addBonus(id: string, bonusGb: number) {
    const rule = await this.prisma.quotaRule.findUnique({ where: { id } });
    if (!rule) throw new NotFoundException('Quota rule not found');
    return this.prisma.quotaRule.update({
      where: { id },
      data: { quotaGb: { increment: bonusGb } },
    });
  }
}
