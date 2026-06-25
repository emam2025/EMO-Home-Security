import { Injectable, OnModuleInit, OnModuleDestroy } from '@nestjs/common';
import { ConfigService } from '@nestjs/config';
import { connect, MqttClient } from 'mqtt';

@Injectable()
export class MqttService implements OnModuleInit, OnModuleDestroy {
  private client: MqttClient | null = null;
  private commandHandlers: Map<string, (payload: string) => void> = new Map();
  private deviceCommandHandlers: Map<
    string,
    (deviceId: string, command: string, params: any) => void
  > = new Map();
  private deviceStatusHandlers: Map<string, (payload: string) => void> = new Map();
  private deviceEventHandlers: Map<string, (payload: string) => void> = new Map();
  private deviceUsageHandlers: Map<string, (payload: string) => void> = new Map();
  private usageHandler: ((deviceId: string, payload: string) => void) | null = null;
  private registrationHandler: ((payload: string) => void) | null = null;

  constructor(private configService: ConfigService) {}

  onModuleInit() {
    const brokerUrl = this.configService.get<string>('MQTT_BROKER_URL') || 'mqtts://localhost:8883';

    // SECURITY: Enforce TLS on port 8883 for production MQTT connections
    if (!brokerUrl.includes('8883') && process.env.NODE_ENV === 'production') {
      throw new Error(
        'MQTT SECURITY VIOLATION: Production broker URL must use TLS port 8883. ' +
          'URL provided but credentials are masked for security.',
      );
    }

    const username = this.configService.get<string>('MQTT_USERNAME');
    const password = this.configService.get<string>('MQTT_PASSWORD');

    this.client = connect(brokerUrl, {
      username,
      password,
      clientId: `emo-cloud-${Math.random().toString(36).substring(2, 10)}`,
      clean: true,
      reconnectPeriod: 5000,
      connectTimeout: 10000,
      // SECURITY: Verify TLS certificate in production
      rejectUnauthorized: process.env.NODE_ENV === 'production',
    });

    this.client.on('connect', () => {
      console.log('MQTT connected to broker');
      this.subscribe('emo/register');
      this.subscribe('emo/+/status');
      this.subscribe('emo/+/events');
      this.subscribe('emo/+/alerts');
      this.subscribe('emo/+/commands');
      this.subscribe('emo/+/policies');
      this.subscribe('emo/+/usage');
    });

    this.client.on('message', (topic, payload) => {
      const message = payload.toString();

      if (topic === 'emo/register') {
        console.log('Device registration received:', message);
        if (this.registrationHandler) {
          this.registrationHandler(message);
        }
        return;
      }

      const statusMatch = topic.match(/^emo\/([a-f0-9-]+)\/status$/);
      if (statusMatch) {
        const handler = this.deviceStatusHandlers.get(statusMatch[1]);
        if (handler) handler(message);
        return;
      }

      const eventsMatch = topic.match(/^emo\/([a-f0-9-]+)\/events$/);
      if (eventsMatch) {
        const handler = this.deviceEventHandlers.get(eventsMatch[1]);
        if (handler) handler(message);
        return;
      }

      const usageMatch = topic.match(/^emo\/([a-f0-9-]+)\/usage$/);
      if (usageMatch) {
        const deviceId = usageMatch[1];
        const perDevice = this.deviceUsageHandlers.get(deviceId);
        if (perDevice) perDevice(message);
        if (this.usageHandler) this.usageHandler(deviceId, message);
        return;
      }

      const cmdMatch = topic.match(/^emo\/([a-f0-9-]+)\/commands$/);
      if (cmdMatch) {
        const deviceId = cmdMatch[1];
        try {
          const parsed = JSON.parse(message);
          const handler = this.deviceCommandHandlers.get(deviceId);
          if (handler) {
            handler(deviceId, parsed.command, parsed.params);
          }
        } catch {
          console.error('Invalid MQTT command JSON from device:', deviceId);
        }
        return;
      }

      const handler = this.commandHandlers.get(topic);
      if (handler) {
        handler(message);
      }
    });

    this.client.on('error', (err) => {
      console.error('MQTT error:', err.message);
    });

    this.client.on('close', () => {
      console.warn('MQTT connection closed');
    });
  }

  onModuleDestroy() {
    if (this.client) {
      this.client.end(true);
    }
  }

  publish(topic: string, message: string) {
    if (this.client?.connected) {
      this.client.publish(topic, message, { qos: 1 });
    } else {
      console.warn('MQTT not connected, cannot publish to:', topic);
    }
  }

  subscribe(topic: string) {
    if (this.client?.connected) {
      this.client.subscribe(topic, { qos: 1 });
    }
  }

  onCommand(topic: string, callback: (payload: string) => void) {
    this.commandHandlers.set(topic, callback);
  }

  onDeviceCommand(
    deviceId: string,
    callback: (deviceId: string, command: string, params: any) => void,
  ) {
    this.deviceCommandHandlers.set(deviceId, callback);
  }

  onDeviceStatus(deviceId: string, callback: (payload: string) => void) {
    this.deviceStatusHandlers.set(deviceId, callback);
  }

  onDeviceEvent(deviceId: string, callback: (payload: string) => void) {
    this.deviceEventHandlers.set(deviceId, callback);
  }

  onDeviceUsage(deviceId: string, callback: (payload: string) => void) {
    this.deviceUsageHandlers.set(deviceId, callback);
  }

  onUsage(callback: (deviceId: string, payload: string) => void) {
    this.usageHandler = callback;
  }

  onRegistration(callback: (payload: string) => void) {
    this.registrationHandler = callback;
  }

  publishToDevice(deviceId: string, command: string, params: Record<string, any> = {}) {
    const topic = `emo/${deviceId}/commands`;
    const payload = JSON.stringify({
      command,
      params,
      id: crypto.randomUUID(),
      timestamp: new Date().toISOString(),
    });
    this.publish(topic, payload);
  }

  publishPolicySync(deviceId: string, payload: Record<string, any>) {
    const topic = `emo/${deviceId}/policies`;
    this.publish(topic, JSON.stringify({ ...payload, updatedAt: new Date().toISOString() }));
  }

  isConnected(): boolean {
    return this.client?.connected ?? false;
  }
}
