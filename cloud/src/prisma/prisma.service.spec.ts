import { Test, TestingModule } from '@nestjs/testing';
import { PrismaService } from './prisma.service';
import { EncryptionService } from '../common/services/encryption.service';

describe('PrismaService', () => {
  let service: PrismaService;

  const mockEncryptionService = {
    encrypt: jest.fn((s: string) => `enc:${s}`),
    decrypt: jest.fn((s: string) => s.replace('enc:', '')),
  };

  beforeEach(async () => {
    const module: TestingModule = await Test.createTestingModule({
      providers: [
        PrismaService,
        { provide: EncryptionService, useValue: mockEncryptionService },
      ],
    }).compile();

    service = module.get<PrismaService>(PrismaService);
  });

  it('should be defined', () => {
    expect(service).toBeDefined();
  });

  it('onModuleInit calls $connect', async () => {
    const connectSpy = jest.spyOn(service, '$connect').mockResolvedValue(undefined as never);
    await service.onModuleInit();
    expect(connectSpy).toHaveBeenCalled();
  });
});
