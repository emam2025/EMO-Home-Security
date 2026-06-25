import { IsIn, IsNotEmpty, IsObject, IsOptional, IsString, IsUUID } from 'class-validator';
import { Expose } from 'class-transformer';

const COMMANDS = [
  'block_device',
  'unblock_device',
  'set_dns',
  'reboot',
  'pause_internet',
  'resume_internet',
  'sync_policies',
  'recover_router',
] as const;

export class SendCommandDto {
  @Expose()
  @IsNotEmpty()
  @IsString()
  @IsIn(COMMANDS)
  command!: string;

  @Expose()
  @IsOptional()
  @IsObject()
  params?: any;

  @Expose()
  @IsNotEmpty()
  @IsUUID()
  deviceId!: string;
}
