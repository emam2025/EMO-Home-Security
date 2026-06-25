import { Controller, Post, Body, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { MqttService } from './mqtt.service';

class SendCommandDto {
  deviceId!: string;
  command!: string;
  params?: Record<string, any>;
}

@Controller('commands')
@UseGuards(JwtAuthGuard)
export class MqttController {
  constructor(private mqttService: MqttService) {}

  @Post()
  sendCommand(@Body() dto: SendCommandDto) {
    this.mqttService.publishToDevice(dto.deviceId, dto.command, dto.params);
    return { sent: true, deviceId: dto.deviceId, command: dto.command };
  }
}
