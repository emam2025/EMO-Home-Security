import { Controller, Get, Put, Body, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { HomeMembershipGuard } from '../common/guards/home-membership.guard';
import { QuotasService } from './quotas.service';

@Controller('homes/:homeId/quotas')
@UseGuards(JwtAuthGuard, HomeMembershipGuard)
export class HomeQuotasController {
  constructor(private quotasService: QuotasService) {}

  @Get()
  findAll(@Param('homeId') homeId: string) {
    return this.quotasService.findByHome(homeId);
  }

  @Put(':profileId')
  update(
    @Param('profileId') profileId: string,
    @Body() body: { quotaGb?: number; period?: string },
  ) {
    return this.quotasService.updateForProfile(profileId, body);
  }
}
