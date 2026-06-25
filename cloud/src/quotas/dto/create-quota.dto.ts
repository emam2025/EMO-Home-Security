import { IsString, IsNumber, IsOptional, IsEnum, IsInt } from 'class-validator';
import { ActionOnExhaust } from '@prisma/client';

export class CreateQuotaDto {
  @IsString()
  profileId!: string;

  @IsNumber()
  quotaGb!: number;

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
