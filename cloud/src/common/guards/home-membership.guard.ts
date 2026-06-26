import { Injectable, CanActivate, ExecutionContext, ForbiddenException } from '@nestjs/common';
import { PrismaService } from '../../prisma/prisma.service';

@Injectable()
export class HomeMembershipGuard implements CanActivate {
  constructor(private prisma: PrismaService) {}

  async canActivate(context: ExecutionContext): Promise<boolean> {
    const request = context.switchToHttp().getRequest();
    const userId = request.user?.id;
    const homeId = request.params?.homeId;

    if (!homeId) return true;

    if (!userId) return false;

    const membership = await this.prisma.homeMember.findUnique({
      where: { homeId_userId: { homeId, userId } },
    });

    if (!membership) {
      throw new ForbiddenException('You are not a member of this home');
    }

    return true;
  }
}
