import { IsString, IsOptional } from 'class-validator';

export class CreateDeviceDto {
  @IsString()
  homeId!: string;

  @IsString()
  routerId!: string;

  @IsString()
  macAddress!: string;

  @IsOptional()
  @IsString()
  firmwareVersion?: string;

  @IsOptional()
  @IsString()
  mqttUsername?: string;

  @IsOptional()
  @IsString()
  mqttPasswordEncrypted?: string;

  @IsOptional()
  @IsString()
  pairingCode?: string;
}
