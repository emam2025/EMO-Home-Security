export class BootError extends Error {
  constructor(missing: string[]) {
    super(`Missing required environment variables: ${missing.join(', ')}`);
    this.name = 'BootError';
  }
}

const REQUIRED_VARS = [
  'DATABASE_URL',
  'JWT_SECRET',
  'DEVICE_PAIRING_SECRET',
  'ENCRYPTION_KEY',
  'MQTT_BROKER_URL',
] as const;

export function validateEnv(config: Record<string, unknown>) {
  const missing = REQUIRED_VARS.filter((key) => !config[key]);
  if (missing.length > 0) {
    throw new BootError(missing);
  }
  return config;
}
