import { Test, TestingModule } from '@nestjs/testing';
import { INestApplication, ValidationPipe } from '@nestjs/common';
import request from 'supertest';
import { AppModule } from './app.module';
import { PrismaService } from './prisma/prisma.service';

describe('HomeMembershipGuard (e2e)', () => {
  let app: INestApplication;
  let prisma: PrismaService;
  let userAToken: string;
  let userBToken: string;
  let homeAId: string;
  let homeBId: string;

  const testEmails = ['userA@test.com', 'userB@test.com', 'test@example.com'];

  async function cleanDatabase() {
    const users = await prisma.user.findMany({ where: { email: { in: testEmails } }, select: { id: true } });
    const ids = users.map(u => u.id);
    if (ids.length > 0) {
      await prisma.home.deleteMany({ where: { ownerId: { in: ids } } });
      await prisma.user.deleteMany({ where: { id: { in: ids } } });
    }
  }

  beforeAll(async () => {
    const moduleFixture: TestingModule = await Test.createTestingModule({
      imports: [AppModule],
    }).compile();

    app = moduleFixture.createNestApplication();
    app.useGlobalPipes(new ValidationPipe({ whitelist: true, transform: true }));
    await app.init();

    prisma = app.get(PrismaService);
    await cleanDatabase();
  });

  afterAll(async () => {
    await cleanDatabase();
    await app.close();
  });

  it('1. Register user A', async () => {
    const res = await request(app.getHttpServer())
      .post('/auth/register')
      .send({ email: 'userA@test.com', password: 'password123', name: 'User A' });
    expect(res.status).toBe(201);
    userAToken = res.body.accessToken;
  });

  it('2. Register user B', async () => {
    const res = await request(app.getHttpServer())
      .post('/auth/register')
      .send({ email: 'userB@test.com', password: 'password123', name: 'User B' });
    expect(res.status).toBe(201);
    userBToken = res.body.accessToken;
  });

  it('3. User A creates home A', async () => {
    const res = await request(app.getHttpServer())
      .post('/homes')
      .set('Authorization', `Bearer ${userAToken}`)
      .send({ name: 'Home A' });
    expect(res.status).toBe(201);
    homeAId = res.body.id;
  });

  it('4. User B creates home B', async () => {
    const res = await request(app.getHttpServer())
      .post('/homes')
      .set('Authorization', `Bearer ${userBToken}`)
      .send({ name: 'Home B' });
    expect(res.status).toBe(201);
    homeBId = res.body.id;
  });

  it('5. User B cannot access home A devices (403)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/homes/${homeAId}/devices`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(403);
  });

  it('6. User B cannot access home A profiles (403)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/homes/${homeAId}/profiles`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(403);
  });

  it('7. User B cannot access home A quotas (403)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/homes/${homeAId}/quotas`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(403);
  });

  it('8. User B cannot access home A usage (403)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/homes/${homeAId}/usage`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(403);
  });

  it('9. User A can access own home devices (200)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/homes/${homeAId}/devices`)
      .set('Authorization', `Bearer ${userAToken}`);
    expect(res.status).toBe(200);
  });

  it('10. User B can access own home devices (200)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/homes/${homeBId}/devices`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(200);
  });

  it('11. User B cannot access home A via direct route /devices/home/:homeId (403)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/devices/home/${homeAId}`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(403);
  });

  it('12. User B cannot access home A via direct route /profiles/home/:homeId (403)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/profiles/home/${homeAId}`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(403);
  });

  it('13. User B cannot access home A via direct route /routers/home/:homeId (403)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/routers/home/${homeAId}`)
      .set('Authorization', `Bearer ${userBToken}`);
    expect(res.status).toBe(403);
  });
});
