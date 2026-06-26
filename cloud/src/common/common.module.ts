import { Global, Module } from '@nestjs/common';
import { HomeMembershipGuard } from './guards/home-membership.guard';
import { EncryptionService } from './services/encryption.service';

@Global()
@Module({
  providers: [HomeMembershipGuard, EncryptionService],
  exports: [HomeMembershipGuard, EncryptionService],
})
export class CommonModule {}
