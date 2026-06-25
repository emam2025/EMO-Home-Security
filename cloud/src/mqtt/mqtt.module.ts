import { Module } from '@nestjs/common';
import { MqttService } from './mqtt.service';
import { MqttController } from './mqtt.controller';
import { MqttUsageService } from './mqtt-usage.service';

@Module({
  controllers: [MqttController],
  providers: [MqttService, MqttUsageService],
  exports: [MqttService, MqttUsageService],
})
export class MqttModule {}
