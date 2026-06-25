import {
  Controller,
  Post,
  Get,
  Patch,
  Delete,
  Body,
  Param,
  UseGuards,
  ValidationPipe,
} from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { NetworkDevicesService } from './network-devices.service';
import { CreateNetworkDeviceDto } from './dto/create-network-device.dto';
import { UpdateNetworkDeviceDto } from './dto/update-network-device.dto';

@Controller('network-devices')
@UseGuards(JwtAuthGuard)
export class NetworkDevicesController {
  constructor(private networkDevicesService: NetworkDevicesService) {}

  @Post()
  create(@Body(new ValidationPipe({ whitelist: true })) dto: CreateNetworkDeviceDto) {
    return this.networkDevicesService.create(dto);
  }

  @Get('home/:homeId')
  findByHome(@Param('homeId') homeId: string) {
    return this.networkDevicesService.findByHome(homeId);
  }

  @Get(':id')
  findOne(@Param('id') id: string) {
    return this.networkDevicesService.findOne(id);
  }

  @Patch(':id')
  update(
    @Param('id') id: string,
    @Body(new ValidationPipe({ whitelist: true })) dto: UpdateNetworkDeviceDto,
  ) {
    return this.networkDevicesService.update(id, dto);
  }

  @Delete(':id')
  remove(@Param('id') id: string) {
    return this.networkDevicesService.remove(id);
  }
}
