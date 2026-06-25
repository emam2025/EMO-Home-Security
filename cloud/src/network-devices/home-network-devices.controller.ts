import { Controller, Post, Get, Body, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { NetworkDevicesService } from './network-devices.service';
import { NetworkDeviceStatus } from '@prisma/client';

@Controller('homes/:homeId/network-devices')
@UseGuards(JwtAuthGuard)
export class HomeNetworkDevicesController {
  constructor(private networkDevicesService: NetworkDevicesService) {}

  @Get()
  findAll(@Param('homeId') homeId: string) {
    return this.networkDevicesService.findByHome(homeId);
  }

  @Post(':deviceId/approve')
  approve(@Param('deviceId') deviceId: string) {
    return this.networkDevicesService.update(deviceId, { status: NetworkDeviceStatus.approved });
  }

  @Post(':deviceId/block')
  block(@Param('deviceId') deviceId: string) {
    return this.networkDevicesService.update(deviceId, { status: NetworkDeviceStatus.blocked });
  }

  @Post(':deviceId/assign')
  assign(@Param('deviceId') deviceId: string, @Body() body: { profileId: string }) {
    return this.networkDevicesService.update(deviceId, { profileId: body.profileId });
  }
}
