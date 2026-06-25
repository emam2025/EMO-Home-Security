import { IsOptional, IsBoolean, IsArray, IsInt, Min, Max, ValidateNested } from 'class-validator';
import { Type } from 'class-transformer';

class TimeSlotDto {
  @IsInt()
  @Min(0)
  @Max(23)
  startHour!: number;

  @IsInt()
  @Min(0)
  @Max(59)
  startMinute!: number;

  @IsInt()
  @Min(0)
  @Max(23)
  endHour!: number;

  @IsInt()
  @Min(0)
  @Max(59)
  endMinute!: number;
}

export class UpdateScheduleDto {
  @IsOptional()
  @IsBoolean()
  enabled?: boolean;

  @IsOptional()
  @IsArray()
  @IsInt({ each: true })
  @Min(0, { each: true })
  @Max(6, { each: true })
  activeDays?: number[];

  @IsOptional()
  @IsArray()
  @ValidateNested({ each: true })
  @Type(() => TimeSlotDto)
  timeSlots?: TimeSlotDto[];
}
