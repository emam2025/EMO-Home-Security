import { Module } from '@nestjs/common';
import { DevicesService } from './devices.service';
import { DevicesController } from './devices.controller';
import { HomeDevicesController } from './home-devices.controller';

@Module({
  controllers: [DevicesController, HomeDevicesController],
  providers: [DevicesService],
  exports: [DevicesService],
})
export class DevicesModule {}
