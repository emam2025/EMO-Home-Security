import { Controller, Post, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { CurrentUser } from '../common/decorators/current-user.decorator';
import { HomesService } from './homes.service';
import { MqttService } from '../mqtt/mqtt.service';

// Type guard to ensure type safety for home with routers
function hasRouters(home: any): home is { routers: any[] } {
  return home && home.routers !== undefined;
}

@Controller('homes/:homeId')
@UseGuards(JwtAuthGuard)
export class HomeResourcesController {
  constructor(
    private homesService: HomesService,
    private mqttService: MqttService,
  ) {}

  @Post('pause')
  async pause(@CurrentUser() user: { id: string }, @Param('homeId') homeId: string) {
    const home = await this.homesService.findOne(homeId, user.id);
    const routers = hasRouters(home) ? home.routers : [];
    for (const router of routers) {
      this.mqttService.publishToDevice(router.id, 'pause_internet');
    }
    return { paused: true };
  }

  @Post('resume')
  async resume(@CurrentUser() user: { id: string }, @Param('homeId') homeId: string) {
    const home = await this.homesService.findOne(homeId, user.id);
    const routers = hasRouters(home) ? home.routers : [];
    for (const router of routers) {
      this.mqttService.publishToDevice(router.id, 'resume_internet');
    }
    return { paused: false };
  }
}
