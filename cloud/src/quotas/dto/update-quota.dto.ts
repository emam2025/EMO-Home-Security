import { IsOptional, IsNumber, IsString, IsEnum, IsInt } from 'class-validator';
import { ActionOnExhaust } from '@prisma/client';

export class UpdateQuotaDto {
  @IsOptional()
  @IsNumber()
  quotaGb?: number;

  @IsOptional()
  @IsNumber()
  consumedGb?: number;

  @IsOptional()
  @IsString()
  period?: string;

  @IsOptional()
  @IsEnum(ActionOnExhaust)
  actionOnExhaust?: ActionOnExhaust;

  @IsOptional()
  @IsInt()
  resetDay?: number;
}
