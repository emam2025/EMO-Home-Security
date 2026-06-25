import { Expose } from 'class-transformer';

export class NotificationResponseDto {
  @Expose()
  id!: string;

  @Expose()
  type!: string;

  @Expose()
  title!: string;

  @Expose()
  body?: string;

  @Expose()
  data?: any;

  @Expose()
  isRead!: boolean;

  @Expose()
  createdAt!: Date;
}
