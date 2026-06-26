import { Controller, Get, Put, Body, Param, UseGuards, ValidationPipe } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { HomeMembershipGuard } from '../common/guards/home-membership.guard';
import { SchedulesService } from './schedules.service';
import { UpdateScheduleDto } from './dto/update-schedule.dto';

@Controller('homes/:homeId/schedules')
@UseGuards(JwtAuthGuard, HomeMembershipGuard)
export class HomeSchedulesController {
  constructor(private schedulesService: SchedulesService) {}

  @Get()
  findAll(@Param('homeId') homeId: string) {
    return this.schedulesService.findByHome(homeId);
  }

  @Put(':profileId')
  update(
    @Param('profileId') profileId: string,
    @Body(new ValidationPipe({ whitelist: true, transform: true })) dto: UpdateScheduleDto,
  ) {
    return this.schedulesService.updateForProfile(profileId, dto);
  }
}
