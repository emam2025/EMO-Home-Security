import { IsEmail, IsOptional, IsString, MinLength } from 'class-validator';
import { Expose } from 'class-transformer';

export class UpdateProfileDto {
  @Expose()
  @IsOptional()
  @IsString()
  name?: string;

  @Expose()
  @IsOptional()
  @IsEmail()
  email?: string;

  @Expose()
  @IsOptional()
  @IsString()
  @MinLength(8)
  password?: string;
}
