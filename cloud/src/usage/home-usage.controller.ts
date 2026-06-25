import { Controller, Get, Query, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { UsageService } from './usage.service';

@Controller('homes/:homeId/usage')
@UseGuards(JwtAuthGuard)
export class HomeUsageController {
  constructor(private usageService: UsageService) {}

  @Get()
  findAll(
    @Param('homeId') homeId: string,
    @Query('period') period?: string,
    @Query('profile_id') profileId?: string,
  ) {
    if (profileId) {
      return this.usageService.profileUsage(profileId);
    }
    if (period === 'history') {
      return this.usageService.homeUsageHistory(homeId);
    }
    return this.usageService.aggregateHomeUsage(homeId);
  }
}
