import { IsOptional, IsString, IsBoolean } from 'class-validator';

export class UpdateRouterDto {
  @IsOptional()
  @IsString()
  ipAddress?: string;

  @IsOptional()
  @IsString()
  firmwareVersion?: string;

  @IsOptional()
  @IsBoolean()
  isManaged?: boolean;
}
