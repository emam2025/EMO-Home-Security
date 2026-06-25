import { NestFactory } from '@nestjs/core';
import { SwaggerModule, DocumentBuilder } from '@nestjs/swagger';
import { AppModule } from './app.module';
import { ValidationPipe } from '@nestjs/common';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);

  app.useGlobalPipes(
    new ValidationPipe({
      whitelist: true,
      forbidNonWhitelisted: true,
      transform: true,
    }),
  );

  const config = new DocumentBuilder()
    .setTitle('EMO Family Internet Manager API')
    .setDescription('REST API for EMO home internet management system')
    .setVersion('1.0')
    .addBearerAuth()
    .addTag('auth', 'Authentication endpoints')
    .addTag('homes', 'Home management')
    .addTag('profiles', 'Family profile management')
    .addTag('devices', 'EMO device management')
    .addTag('network-devices', 'Network device management')
    .addTag('routers', 'Router management')
    .addTag('quotas', 'Data quota rules')
    .addTag('schedules', 'Time schedule management')
    .addTag('usage', 'Usage statistics')
    .addTag('notifications', 'User notifications')
    .addTag('alerts', 'System alerts')
    .addTag('policies', 'Policy management')
    .build();

  const document = SwaggerModule.createDocument(app, config);
  SwaggerModule.setup('api', app, document);

  await app.listen(3000);
}
bootstrap();
