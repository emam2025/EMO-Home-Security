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
import { HomeMembershipGuard } from '../common/guards/home-membership.guard';
import { DevicesService } from './devices.service';
import { CreateDeviceDto } from './dto/create-device.dto';
import { UpdateDeviceDto } from './dto/update-device.dto';

@Controller('devices')
@UseGuards(JwtAuthGuard)
export class DevicesController {
  constructor(private devicesService: DevicesService) {}

  @Post()
  create(@Body(new ValidationPipe({ whitelist: true })) dto: CreateDeviceDto) {
    return this.devicesService.create(dto);
  }

  @Get('home/:homeId')
  @UseGuards(HomeMembershipGuard)
  findByHome(@Param('homeId') homeId: string) {
    return this.devicesService.findByHome(homeId);
  }

  @Get('router/:routerId')
  findByRouter(@Param('routerId') routerId: string) {
    return this.devicesService.findByRouter(routerId);
  }

  @Get(':id')
  findOne(@Param('id') id: string) {
    return this.devicesService.findOne(id);
  }

  @Patch(':id')
  update(
    @Param('id') id: string,
    @Body(new ValidationPipe({ whitelist: true })) dto: UpdateDeviceDto,
  ) {
    return this.devicesService.update(id, dto);
  }

  @Delete(':id')
  remove(@Param('id') id: string) {
    return this.devicesService.remove(id);
  }
}
