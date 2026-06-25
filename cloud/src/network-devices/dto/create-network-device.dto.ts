import { IsString, IsOptional, IsEnum } from 'class-validator';
import { NetworkDeviceStatus } from '@prisma/client';

export class CreateNetworkDeviceDto {
  @IsString()
  homeId!: string;

  @IsString()
  routerId!: string;

  @IsOptional()
  @IsString()
  profileId?: string;

  @IsString()
  macAddress!: string;

  @IsOptional()
  @IsString()
  ipAddress?: string;

  @IsOptional()
  @IsString()
  hostname?: string;

  @IsOptional()
  @IsString()
  vendor?: string;

  @IsOptional()
  @IsEnum(NetworkDeviceStatus)
  status?: NetworkDeviceStatus;
}
