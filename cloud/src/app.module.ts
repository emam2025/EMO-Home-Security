import { Module } from '@nestjs/common';
import { ConfigModule } from '@nestjs/config';
import { PrismaModule } from './prisma/prisma.module';
import { CommonModule } from './common/common.module';
import { AuthModule } from './auth/auth.module';
import { UsersModule } from './users/users.module';
import { HomesModule } from './homes/homes.module';
import { ProfilesModule } from './profiles/profiles.module';
import { DevicesModule } from './devices/devices.module';
import { RoutersModule } from './routers/routers.module';
import { QuotasModule } from './quotas/quotas.module';
import { SchedulesModule } from './schedules/schedules.module';
import { NetworkDevicesModule } from './network-devices/network-devices.module';
import { NotificationsModule } from './notifications/notifications.module';
import { MqttModule } from './mqtt/mqtt.module';
import { UsageModule } from './usage/usage.module';
import { HomeResourcesController } from './homes/home-resources.controller';

@Module({
  controllers: [HomeResourcesController],
  imports: [
    ConfigModule.forRoot({ isGlobal: true, envFilePath: '../.env' }),
    CommonModule,
    PrismaModule,
    AuthModule,
    UsersModule,
    HomesModule,
    ProfilesModule,
    DevicesModule,
    RoutersModule,
    QuotasModule,
    SchedulesModule,
    NetworkDevicesModule,
    NotificationsModule,
    MqttModule,
    UsageModule,
  ],
})
export class AppModule {}
