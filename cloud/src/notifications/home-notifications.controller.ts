import { Controller, Get, Post, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { CurrentUser } from '../common/decorators/current-user.decorator';
import { NotificationsService } from './notifications.service';

@Controller('homes/:homeId/notifications')
@UseGuards(JwtAuthGuard)
export class HomeNotificationsController {
  constructor(private notificationsService: NotificationsService) {}

  @Get()
  findAll(@CurrentUser() user: { id: string }) {
    return this.notificationsService.findByUser(user.id);
  }

  @Post(':notificationId/read')
  markRead(@Param('notificationId') notificationId: string) {
    return this.notificationsService.markRead(notificationId);
  }
}
