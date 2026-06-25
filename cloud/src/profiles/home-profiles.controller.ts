import {
  Controller,
  Post,
  Get,
  Put,
  Delete,
  Body,
  Param,
  UseGuards,
  ValidationPipe,
} from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { ProfilesService } from './profiles.service';
import { CreateProfileDto } from './dto/create-profile.dto';
import { UpdateProfileDto } from './dto/update-profile.dto';

@Controller('homes/:homeId/profiles')
@UseGuards(JwtAuthGuard)
export class HomeProfilesController {
  constructor(private profilesService: ProfilesService) {}

  @Get()
  findAll(@Param('homeId') homeId: string) {
    return this.profilesService.findByHome(homeId);
  }

  @Post()
  create(
    @Param('homeId') homeId: string,
    @Body(new ValidationPipe({ whitelist: true })) dto: CreateProfileDto,
  ) {
    return this.profilesService.create(homeId, dto);
  }

  @Get(':profileId')
  findOne(@Param('profileId') profileId: string) {
    return this.profilesService.findOne(profileId);
  }

  @Put(':profileId')
  update(
    @Param('profileId') profileId: string,
    @Body(new ValidationPipe({ whitelist: true })) dto: UpdateProfileDto,
  ) {
    return this.profilesService.update(profileId, dto);
  }

  @Delete(':profileId')
  remove(@Param('profileId') profileId: string) {
    return this.profilesService.remove(profileId);
  }
}
