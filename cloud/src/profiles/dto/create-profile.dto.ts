import { IsString, IsOptional } from 'class-validator';

export class CreateProfileDto {
  @IsString()
  homeId!: string;

  @IsString()
  name!: string;

  @IsOptional()
  @IsString()
  avatar?: string;
}
