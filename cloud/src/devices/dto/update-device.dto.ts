import { IsOptional, IsString, IsBoolean } from 'class-validator';

export class UpdateDeviceDto {
  @IsOptional()
  @IsString()
  firmwareVersion?: string;

  @IsOptional()
  @IsBoolean()
  isOnline?: boolean;
}
