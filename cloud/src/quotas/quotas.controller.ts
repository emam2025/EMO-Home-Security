import { Controller, Post, Get, Patch, Delete, Body, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../common/guards/jwt-auth.guard';
import { QuotasService } from './quotas.service';
import { CreateQuotaDto } from './dto/create-quota.dto';
import { UpdateQuotaDto } from './dto/update-quota.dto';
import { BonusQuotaDto } from './dto/bonus-quota.dto';

@Controller('quotas')
@UseGuards(JwtAuthGuard)
export class QuotasController {
  constructor(private quotasService: QuotasService) {}

  @Post()
  create(@Body() dto: CreateQuotaDto) {
    return this.quotasService.create(dto);
  }

  @Get('profile/:profileId')
  findByProfile(@Param('profileId') profileId: string) {
    return this.quotasService.findByProfile(profileId);
  }

  @Get(':id')
  findOne(@Param('id') id: string) {
    return this.quotasService.findOne(id);
  }

  @Patch(':id')
  update(@Param('id') id: string, @Body() dto: UpdateQuotaDto) {
    return this.quotasService.update(id, dto);
  }

  @Post(':id/bonus')
  addBonus(@Param('id') id: string, @Body() dto: BonusQuotaDto) {
    return this.quotasService.addBonus(id, dto.bonusGb);
  }

  @Delete(':id')
  remove(@Param('id') id: string) {
    return this.quotasService.remove(id);
  }
}
