import { IsNumber } from 'class-validator';

export class BonusQuotaDto {
  @IsNumber()
  bonusGb!: number;
}
