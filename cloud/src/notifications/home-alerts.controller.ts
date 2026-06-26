import { Controller, Get, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { HomeMembershipGuard } from '../common/guards/home-membership.guard';
import { CurrentUser } from '../common/decorators/current-user.decorator';
import { NotificationsService } from './notifications.service';

@Controller('homes/:homeId/alerts')
@UseGuards(JwtAuthGuard, HomeMembershipGuard)
export class HomeAlertsController {
  constructor(private notificationsService: NotificationsService) {}

  @Get()
  findAll(@CurrentUser() user: { id: string }) {
    return this.notificationsService.findByUser(user.id);
  }
}
