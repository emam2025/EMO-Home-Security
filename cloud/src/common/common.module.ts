import { Global, Module } from '@nestjs/common';
import { HomeMembershipGuard } from './guards/home-membership.guard';

@Global()
@Module({
  providers: [HomeMembershipGuard],
  exports: [HomeMembershipGuard],
})
export class CommonModule {}
