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
import { RoutersService } from './routers.service';
import { CreateRouterDto } from './dto/create-router.dto';
import { UpdateRouterDto } from './dto/update-router.dto';

@Controller('routers')
@UseGuards(JwtAuthGuard)
export class RoutersController {
  constructor(private routersService: RoutersService) {}

  @Post()
  create(@Body(new ValidationPipe({ whitelist: true })) dto: CreateRouterDto) {
    return this.routersService.create(dto);
  }

  @Get('home/:homeId')
  @UseGuards(HomeMembershipGuard)
  findByHome(@Param('homeId') homeId: string) {
    return this.routersService.findByHome(homeId);
  }

  @Get(':id')
  findOne(@Param('id') id: string) {
    return this.routersService.findOne(id);
  }

  @Patch(':id')
  update(
    @Param('id') id: string,
    @Body(new ValidationPipe({ whitelist: true })) dto: UpdateRouterDto,
  ) {
    return this.routersService.update(id, dto);
  }

  @Delete(':id')
  remove(@Param('id') id: string) {
    return this.routersService.remove(id);
  }
}
