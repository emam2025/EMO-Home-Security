import { Controller, Post, Get, Patch, Delete, Body, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { CurrentUser } from '../common/decorators/current-user.decorator';
import { HomesService } from './homes.service';
import { CreateHomeDto } from './dto/create-home.dto';
import { UpdateHomeDto } from './dto/update-home.dto';

@Controller('homes')
@UseGuards(JwtAuthGuard)
export class HomesController {
  constructor(private homesService: HomesService) {}

  @Post()
  create(@CurrentUser() user: { id: string }, @Body() dto: CreateHomeDto) {
    return this.homesService.create(user.id, dto);
  }

  @Get()
  findAll(@CurrentUser() user: { id: string }) {
    return this.homesService.findAll(user.id);
  }

  @Get(':id')
  findOne(@CurrentUser() user: { id: string }, @Param('id') id: string) {
    return this.homesService.findOne(id, user.id);
  }

  @Patch(':id')
  update(@CurrentUser() user: { id: string }, @Param('id') id: string, @Body() dto: UpdateHomeDto) {
    return this.homesService.update(id, dto, user.id);
  }

  @Delete(':id')
  remove(@CurrentUser() user: { id: string }, @Param('id') id: string) {
    return this.homesService.remove(id, user.id);
  }

  @Post(':id/members')
  addMember(
    @CurrentUser() user: { id: string },
    @Param('id') id: string,
    @Body() body: { userId: string; role: string },
  ) {
    return this.homesService.addMember(id, user.id, body.userId, body.role);
  }
}
