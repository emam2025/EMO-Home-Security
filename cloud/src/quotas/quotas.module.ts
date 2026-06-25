import { Module } from '@nestjs/common';
import { QuotasService } from './quotas.service';
import { QuotasController } from './quotas.controller';
import { HomeQuotasController } from './home-quotas.controller';

@Module({
  controllers: [QuotasController, HomeQuotasController],
  providers: [QuotasService],
  exports: [QuotasService],
})
export class QuotasModule {}
