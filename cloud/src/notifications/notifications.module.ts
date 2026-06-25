import { Module } from '@nestjs/common';
import { NotificationsService } from './notifications.service';
import { NotificationsController } from './notifications.controller';
import { HomeNotificationsController } from './home-notifications.controller';
import { HomeAlertsController } from './home-alerts.controller';

@Module({
  controllers: [NotificationsController, HomeNotificationsController, HomeAlertsController],
  providers: [NotificationsService],
  exports: [NotificationsService],
})
export class NotificationsModule {}
