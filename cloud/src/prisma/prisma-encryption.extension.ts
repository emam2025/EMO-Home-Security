import { EncryptionService } from '../common/services/encryption.service';

const ENCRYPTED_FIELDS = [
  'credentialsEncrypted',
  'factoryCredentialsEncrypted',
  'mqttPasswordEncrypted',
] as const;

type EncryptedField = typeof ENCRYPTED_FIELDS[number];

function encryptFields(data: Record<string, unknown>, service: EncryptionService) {
  for (const field of ENCRYPTED_FIELDS) {
    if (data[field] && typeof data[field] === 'string') {
      data[field] = service.encrypt(data[field] as string);
    }
  }
}

export function createEncryptionExtension(service: EncryptionService) {
  return {
    name: 'encryption',
    query: {
      router: {
        async create({ args, query }: { args: any; query: any }) {
          if (args.data) {
            encryptFields(args.data as Record<string, unknown>, service);
          }
          return query(args);
        },
        async update({ args, query }: { args: any; query: any }) {
          if (args.data) {
            encryptFields(args.data as Record<string, unknown>, service);
          }
          return query(args);
        },
      },
      device: {
        async create({ args, query }: { args: any; query: any }) {
          if (args.data) {
            encryptFields(args.data as Record<string, unknown>, service);
          }
          return query(args);
        },
        async update({ args, query }: { args: any; query: any }) {
          if (args.data) {
            encryptFields(args.data as Record<string, unknown>, service);
          }
          return query(args);
        },
      },
    },
    result: {
      router: {
        credentialsEncrypted: {
          needs: { credentialsEncrypted: true },
          compute(router: { credentialsEncrypted: string | null }) {
            return router.credentialsEncrypted
              ? service.decrypt(router.credentialsEncrypted)
              : null;
          },
        },
        factoryCredentialsEncrypted: {
          needs: { factoryCredentialsEncrypted: true },
          compute(router: { factoryCredentialsEncrypted: string | null }) {
            return router.factoryCredentialsEncrypted
              ? service.decrypt(router.factoryCredentialsEncrypted)
              : null;
          },
        },
      },
      device: {
        mqttPasswordEncrypted: {
          needs: { mqttPasswordEncrypted: true },
          compute(device: { mqttPasswordEncrypted: string | null }) {
            return device.mqttPasswordEncrypted
              ? service.decrypt(device.mqttPasswordEncrypted)
              : null;
          },
        },
      },
    },
  };
}
