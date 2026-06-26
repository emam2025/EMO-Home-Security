import { Controller, Get, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { HomeMembershipGuard } from '../common/guards/home-membership.guard';
import { DevicesService } from './devices.service';

@Controller('homes/:homeId/devices')
@UseGuards(JwtAuthGuard, HomeMembershipGuard)
export class HomeDevicesController {
  constructor(private devicesService: DevicesService) {}

  @Get()
  findAll(@Param('homeId') homeId: string) {
    return this.devicesService.findByHome(homeId);
  }
}
