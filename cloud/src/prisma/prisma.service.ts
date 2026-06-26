import { Injectable, OnModuleInit, OnModuleDestroy } from '@nestjs/common';
import { PrismaClient } from '@prisma/client';
import { EncryptionService } from '../common/services/encryption.service';
import { createEncryptionExtension } from './prisma-encryption.extension';

@Injectable()
export class PrismaService extends PrismaClient implements OnModuleInit, OnModuleDestroy {
  constructor(encryptionService: EncryptionService) {
    super();

    // PrismaClient is a Proxy that intercepts model-name property access
    // before TypeScript getters can fire. Object.defineProperty creates an
    // own property on the Proxy target which DOES take precedence when the
    // Proxy's get trap checks for it.
    const extended = this.$extends(
      createEncryptionExtension(encryptionService),
    ) as unknown as PrismaClient;

    Object.defineProperty(this, 'router', {
      value: extended.router,
      writable: true,
      configurable: true,
    });
    Object.defineProperty(this, 'device', {
      value: extended.device,
      writable: true,
      configurable: true,
    });
  }

  async onModuleInit() { await this.$connect(); }
  async onModuleDestroy() { await this.$disconnect(); }
}
