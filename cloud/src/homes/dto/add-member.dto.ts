import { IsEnum, IsNotEmpty, IsUUID } from 'class-validator';
import { Expose } from 'class-transformer';

enum HomeMemberRole {
  parent = 'parent',
  child = 'child',
  admin = 'admin',
}

export class AddMemberDto {
  @Expose()
  @IsNotEmpty()
  @IsUUID()
  userId!: string;

  @Expose()
  @IsNotEmpty()
  @IsEnum(HomeMemberRole)
  role!: string;
}
