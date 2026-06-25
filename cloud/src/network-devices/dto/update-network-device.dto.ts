import { IsOptional, IsString, IsEnum } from 'class-validator';
import { NetworkDeviceStatus } from '@prisma/client';

export class UpdateNetworkDeviceDto {
  @IsOptional()
  @IsString()
  profileId?: string;

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
