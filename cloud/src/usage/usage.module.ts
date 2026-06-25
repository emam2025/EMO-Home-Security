import { Module } from '@nestjs/common';
import { UsageController } from './usage.controller';
import { UsageService } from './usage.service';
import { HomeUsageController } from './home-usage.controller';

@Module({
  controllers: [UsageController, HomeUsageController],
  providers: [UsageService],
  exports: [UsageService],
})
export class UsageModule {}
