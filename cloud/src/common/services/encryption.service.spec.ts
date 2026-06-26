import { EncryptionService } from './encryption.service';

const TEST_KEY = '748e98591eaf86e79de5d33e3ccf15f4e11cbc1734618d1b559d1f7d9d8eb4cb';

function mockConfigService(key: string = TEST_KEY) {
  return {
    getOrThrow: () => key,
  } as any;
}

describe('EncryptionService', () => {
  let service: EncryptionService;

  beforeEach(() => {
    service = new EncryptionService(mockConfigService());
  });

  it('encrypt returns base64 string', () => {
    const result = service.encrypt('hello');
    expect(typeof result).toBe('string');
    expect(result.length).toBeGreaterThan(0);
  });

  it('decrypt returns original plaintext', () => {
    const original = 'router-admin-password-123!';
    const encrypted = service.encrypt(original);
    const decrypted = service.decrypt(encrypted);
    expect(decrypted).toBe(original);
  });

  it('encrypt produces different output for same input (random IV)', () => {
    const plaintext = 'same-value';
    const a = service.encrypt(plaintext);
    const b = service.encrypt(plaintext);
    expect(a).not.toBe(b);
  });

  it('round-trip works for empty string', () => {
    const encrypted = service.encrypt('');
    const decrypted = service.decrypt(encrypted);
    expect(decrypted).toBe('');
  });

  it('round-trip works for special characters', () => {
    const original = 'p@ssw0rd!~#$%^&*()_+{}|:<>?';
    const encrypted = service.encrypt(original);
    const decrypted = service.decrypt(encrypted);
    expect(decrypted).toBe(original);
  });

  it('round-trip works for long string', () => {
    const original = 'a'.repeat(10000);
    const encrypted = service.encrypt(original);
    const decrypted = service.decrypt(encrypted);
    expect(decrypted).toBe(original);
  });

  it('throws on tampered ciphertext', () => {
    const encrypted = service.encrypt('secret-data');
    const buf = Buffer.from(encrypted, 'base64');
    buf[20] ^= 0xff;
    const tampered = buf.toString('base64');
    expect(() => service.decrypt(tampered)).toThrow();
  });

  it('throws on invalid base64', () => {
    expect(() => service.decrypt('not-base64!!!')).toThrow();
  });
});
