import { Module } from '@nestjs/common';
import { NetworkDevicesService } from './network-devices.service';
import { NetworkDevicesController } from './network-devices.controller';
import { HomeNetworkDevicesController } from './home-network-devices.controller';

@Module({
  controllers: [NetworkDevicesController, HomeNetworkDevicesController],
  providers: [NetworkDevicesService],
  exports: [NetworkDevicesService],
})
export class NetworkDevicesModule {}
