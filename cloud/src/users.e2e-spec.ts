import { Test, TestingModule } from '@nestjs/testing';
import { INestApplication, ValidationPipe } from '@nestjs/common';
import request from 'supertest';
import { AppModule } from './app.module';
import { PrismaService } from './prisma/prisma.service';

function decodeJwt(token: string): any {
  const payload = token.split('.')[1];
  const decoded = Buffer.from(payload, 'base64url').toString('utf8');
  return JSON.parse(decoded);
}

describe('Users RBAC (e2e)', () => {
  let app: INestApplication;
  let prisma: PrismaService;

  const emailA = 'rbac-user-a@test.com';
  const emailB = 'rbac-user-b@test.com';
  const emailAdmin = 'rbac-admin@test.com';

  let tokenA: string;
  let tokenB: string;
  let tokenAdmin: string;
  let userIdA: string;
  let userIdB: string;

  beforeAll(async () => {
    const moduleFixture: TestingModule = await Test.createTestingModule({
      imports: [AppModule],
    }).compile();

    app = moduleFixture.createNestApplication();
    app.useGlobalPipes(new ValidationPipe({ whitelist: true, transform: true }));
    await app.init();

    prisma = app.get(PrismaService);

    // Clean up
    for (const email of [emailA, emailB, emailAdmin]) {
      const u = await prisma.user.findUnique({ where: { email } });
      if (u) await prisma.user.delete({ where: { id: u.id } });
    }

    // Register user A
    const resA = await request(app.getHttpServer())
      .post('/auth/register')
      .send({ email: emailA, password: 'password123', name: 'UserA' });
    tokenA = resA.body.accessToken;
    userIdA = decodeJwt(tokenA).sub;

    // Register user B
    const resB = await request(app.getHttpServer())
      .post('/auth/register')
      .send({ email: emailB, password: 'password123', name: 'UserB' });
    tokenB = resB.body.accessToken;
    userIdB = decodeJwt(tokenB).sub;

    // Register admin (manually promote to admin)
    const resAdmin = await request(app.getHttpServer())
      .post('/auth/register')
      .send({ email: emailAdmin, password: 'password123', name: 'Admin' });
    tokenAdmin = resAdmin.body.accessToken;
    await prisma.user.update({
      where: { email: emailAdmin },
      data: { role: 'admin' },
    });
  });

  afterAll(async () => {
    for (const email of [emailA, emailB, emailAdmin]) {
      const u = await prisma.user.findUnique({ where: { email } });
      if (u) await prisma.user.delete({ where: { id: u.id } });
    }
    await app.close();
  });

  it('1. GET /users returns 200 for admin', async () => {
    const res = await request(app.getHttpServer())
      .get('/users')
      .set('Authorization', `Bearer ${tokenAdmin}`);
    expect(res.status).toBe(200);
    expect(Array.isArray(res.body)).toBe(true);
  });

  it('2. GET /users returns 403 for non-admin', async () => {
    const res = await request(app.getHttpServer())
      .get('/users')
      .set('Authorization', `Bearer ${tokenA}`);
    expect(res.status).toBe(403);
  });

  it('3. GET /users/me returns own profile', async () => {
    const res = await request(app.getHttpServer())
      .get('/users/me')
      .set('Authorization', `Bearer ${tokenA}`);
    expect(res.status).toBe(200);
    expect(res.body.email).toBe(emailA);
  });

  it('4. GET /users/:id returns own profile (user A)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenA}`);
    expect(res.status).toBe(200);
    expect(res.body.id).toBe(userIdA);
  });

  it('5. GET /users/:id returns 403 for other user (B accessing A)', async () => {
    const res = await request(app.getHttpServer())
      .get(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenB}`);
    expect(res.status).toBe(403);
  });

  it('6. GET /users/:id works for admin accessing other user', async () => {
    const res = await request(app.getHttpServer())
      .get(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenAdmin}`);
    expect(res.status).toBe(200);
    expect(res.body.id).toBe(userIdA);
  });

  it('7. PATCH /users/:id updates own profile', async () => {
    const res = await request(app.getHttpServer())
      .patch(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenA}`)
      .send({ name: 'UserA-Updated' });
    expect(res.status).toBe(200);
    expect(res.body.name).toBe('UserA-Updated');
  });

  it('8. PATCH /users/:id returns 403 for other user', async () => {
    const res = await request(app.getHttpServer())
      .patch(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenB}`)
      .send({ name: 'Hacked' });
    expect(res.status).toBe(403);
  });

  it('9. DELETE /users/:id returns 403 for other user', async () => {
    const res = await request(app.getHttpServer())
      .delete(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenB}`);
    expect(res.status).toBe(403);
  });

  it('10. DELETE /users/:id works for admin', async () => {
    const res = await request(app.getHttpServer())
      .delete(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenAdmin}`);
    expect(res.status).toBe(200);

    // Verify deleted
    const check = await request(app.getHttpServer())
      .get(`/users/${userIdA}`)
      .set('Authorization', `Bearer ${tokenAdmin}`);
    expect(check.status).toBe(404);
  });
});
