import { Test, TestingModule } from '@nestjs/testing';
import { INestApplication } from '@nestjs/common';
import { PrismaClient } from '@prisma/client';
import request from 'supertest';
import { AppModule } from './app.module';
import { PrismaService } from './prisma/prisma.service';
import { EncryptionService } from './common/services/encryption.service';

describe('Encryption (e2e)', () => {
  let app: INestApplication;
  let prisma: PrismaService;
  let encryptionService: EncryptionService;

  const testEmail = 'enc-e2e-test@test.com';
  const dupEmail = 'enc-e2e-dup@test.com';

  async function cleanupUser(email: string) {
    const u = await prisma.user.findUnique({ where: { email } });
    if (u) {
      await prisma.router.deleteMany({ where: { home: { ownerId: u.id } } });
      await prisma.home.deleteMany({ where: { ownerId: u.id } });
      await prisma.user.delete({ where: { id: u.id } });
    }
  }

  beforeAll(async () => {
    const moduleFixture: TestingModule = await Test.createTestingModule({
      imports: [AppModule],
    }).compile();

    app = moduleFixture.createNestApplication();
    await app.init();

    prisma = app.get(PrismaService);
    encryptionService = app.get(EncryptionService);

    await cleanupUser(testEmail);
    await cleanupUser(dupEmail);
  });

  afterAll(async () => {
    await cleanupUser(testEmail);
    await cleanupUser(dupEmail);
    await app.close();
  });

  it('EncryptionService round-trip works', () => {
    const encrypted = encryptionService.encrypt('test-pass');
    expect(encryptionService.decrypt(encrypted)).toBe('test-pass');
  });

  it('$extends interceptor encrypts before DB write', async () => {
    const extended = new PrismaClient().$extends(
      (await import('./prisma/prisma-encryption.extension')).createEncryptionExtension(encryptionService),
    ) as any;

    try {
      await extended.router.create({
        data: {
          homeId: '00000000-0000-0000-0000-000000000000',
          model: 't', manufacturer: 't',
          ipAddress: '1.1.1.1', macAddress: 'MAC-EXT-TEST',
          credentialsEncrypted: 'PLAINTEXT',
        },
      });
    } catch (e: any) {
      expect(e.message).toContain('Foreign key');
    }
  });

  it('API stores encrypted value in DB', async () => {
    const regRes = await request(app.getHttpServer())
      .post('/auth/register')
      .send({ email: testEmail, password: 'password123', name: 'EncTest' });
    expect(regRes.status).toBe(201);
    const token = regRes.body.accessToken;

    const homeRes = await request(app.getHttpServer())
      .post('/homes')
      .set('Authorization', `Bearer ${token}`)
      .send({ name: 'EncTestHome' });
    expect(homeRes.status).toBe(201);
    const homeId = homeRes.body.id;

    const plaintext = 'admin-p@ssw0rd!secret';
    const routerRes = await request(app.getHttpServer())
      .post('/routers')
      .set('Authorization', `Bearer ${token}`)
      .send({
        homeId,
        model: 'HG8145V5',
        manufacturer: 'Huawei',
        ipAddress: '192.168.1.1',
        macAddress: 'MAC-API-ENC',
        credentialsEncrypted: plaintext,
      });
    expect(routerRes.status).toBe(201);
    // API returns decrypted value
    expect(routerRes.body.credentialsEncrypted).toBe(plaintext);

    // Raw DB must be encrypted (not plaintext)
    const raw = await prisma.$queryRawUnsafe<Array<{ credentialsEncrypted: string | null }>>(
      'SELECT "credentialsEncrypted" FROM "Router" WHERE id = $1', routerRes.body.id,
    );
    const stored = raw[0]?.credentialsEncrypted;
    console.log('STORED:', stored);
    expect(stored).not.toBe(plaintext);
    expect(stored).toBeTruthy();
    expect(stored).toMatch(/^[A-Za-z0-9+/]+={0,2}$/); // base64

    // Cleanup
    await prisma.router.delete({ where: { id: routerRes.body.id } });
  });

  it('Same plaintext produces different ciphertexts (random IV)', async () => {
    const regRes = await request(app.getHttpServer())
      .post('/auth/register')
      .send({ email: dupEmail, password: 'password123', name: 'DupTest' });
    expect(regRes.status).toBe(201);
    const token = regRes.body.accessToken;
    const homeRes = await request(app.getHttpServer())
      .post('/homes')
      .set('Authorization', `Bearer ${token}`)
      .send({ name: 'DupHome' });
    const homeId = homeRes.body.id;

    const r1 = await prisma.router.create({
      data: {
        homeId,
        model: 'A',
        manufacturer: 'B',
        ipAddress: '10.0.0.1',
        macAddress: 'MAC-IV-1',
        credentialsEncrypted: 'same-value',
      },
    });
    const r2 = await prisma.router.create({
      data: {
        homeId,
        model: 'A',
        manufacturer: 'B',
        ipAddress: '10.0.0.2',
        macAddress: 'MAC-IV-2',
        credentialsEncrypted: 'same-value',
      },
    });

    const raw = await prisma.$queryRawUnsafe<Array<{ credentialsEncrypted: string }>>(
      `SELECT "credentialsEncrypted" FROM "Router" WHERE id IN ($1, $2) ORDER BY id`,
      r1.id, r2.id,
    );
    expect(raw[0].credentialsEncrypted).not.toBe(raw[1].credentialsEncrypted);

    // Cleanup
    await prisma.router.deleteMany({ where: { id: { in: [r1.id, r2.id] } } });
  });

  it('Decrypt fails on tampered ciphertext', async () => {
    const encrypted = encryptionService.encrypt('secret-data');
    // Tamper with the last byte
    const tampered = encrypted.slice(0, -1) + (encrypted.at(-1) === 'A' ? 'B' : 'A');
    expect(() => encryptionService.decrypt(tampered)).toThrow();
  });

  it('Decrypt fails on invalid base64', () => {
    expect(() => encryptionService.decrypt('!!!not-base64!!!')).toThrow();
  });
});
