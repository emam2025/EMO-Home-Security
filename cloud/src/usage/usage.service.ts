import { Injectable } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service';

@Injectable()
export class UsageService {
  constructor(private prisma: PrismaService) {}

  async aggregateHomeUsage(homeId: string) {
    const profiles = await this.prisma.profile.findMany({
      where: { homeId },
      include: {
        usageLogs: true,
      },
    });

    return profiles.map((profile) => {
      const totalDownloaded = profile.usageLogs.reduce(
        (sum, log) => sum + Number(log.bytesDownloaded),
        0,
      );
      const totalUploaded = profile.usageLogs.reduce(
        (sum, log) => sum + Number(log.bytesUploaded),
        0,
      );
      return {
        profileId: profile.id,
        profileName: profile.name,
        totalDownloaded,
        totalUploaded,
        totalBytes: totalDownloaded + totalUploaded,
      };
    });
  }

  async homeUsageHistory(homeId: string, from?: string, to?: string) {
    const profiles = await this.prisma.profile.findMany({
      where: { homeId },
      select: { id: true },
    });

    const profileIds = profiles.map((p) => p.id);
    const where: any = { profileId: { in: profileIds } };

    if (from || to) {
      where.loggedAt = {};
      if (from) where.loggedAt.gte = new Date(from);
      if (to) where.loggedAt.lte = new Date(to);
    }

    return this.prisma.usageLog.findMany({
      where,
      include: { profile: { select: { id: true, name: true } } },
      orderBy: { loggedAt: 'asc' },
    });
  }

  async profileUsage(profileId: string) {
    const profile = await this.prisma.profile.findUnique({
      where: { id: profileId },
    });

    const logs = await this.prisma.usageLog.findMany({
      where: { profileId },
      orderBy: { loggedAt: 'desc' },
    });

    const totalDownloaded = logs.reduce((sum, log) => sum + Number(log.bytesDownloaded), 0);
    const totalUploaded = logs.reduce((sum, log) => sum + Number(log.bytesUploaded), 0);

    return {
      profile,
      totalDownloaded,
      totalUploaded,
      totalBytes: totalDownloaded + totalUploaded,
      logs,
    };
  }
}
