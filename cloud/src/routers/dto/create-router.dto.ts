import { IsString, IsOptional } from 'class-validator';

export class CreateRouterDto {
  @IsString()
  homeId!: string;

  @IsString()
  model!: string;

  @IsString()
  manufacturer!: string;

  @IsString()
  ipAddress!: string;

  @IsString()
  macAddress!: string;

  @IsOptional()
  @IsString()
  serialNumber?: string;

  @IsOptional()
  @IsString()
  credentialsEncrypted?: string;

  @IsOptional()
  @IsString()
  factoryCredentialsEncrypted?: string;
}
