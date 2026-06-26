import { Controller, Get, Param, Query, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { HomeMembershipGuard } from '../common/guards/home-membership.guard';
import { UsageService } from './usage.service';

@Controller()
@UseGuards(JwtAuthGuard)
export class UsageController {
  constructor(private usageService: UsageService) {}

  @Get('homes/:homeId/usage')
  @UseGuards(HomeMembershipGuard)
  aggregateHomeUsage(@Param('homeId') homeId: string) {
    return this.usageService.aggregateHomeUsage(homeId);
  }

  @Get('homes/:homeId/usage/history')
  @UseGuards(HomeMembershipGuard)
  homeUsageHistory(
    @Param('homeId') homeId: string,
    @Query('from') from?: string,
    @Query('to') to?: string,
  ) {
    return this.usageService.homeUsageHistory(homeId, from, to);
  }

  @Get('profiles/:profileId/usage')
  profileUsage(@Param('profileId') profileId: string) {
    return this.usageService.profileUsage(profileId);
  }
}
