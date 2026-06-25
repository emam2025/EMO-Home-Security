enum NotificationType {
  deviceApprovalRequest,
  quotaExceeded,
  newDevice,
  deviceOffline,
  alert,
  unknown,
}

class NotificationItem {
  final String id;
  final String homeId;
  final NotificationType type;
  final String title;
  final String body;
  final bool isRead;
  final DateTime createdAt;
  final Map<String, dynamic>? data;

  const NotificationItem({
    required this.id,
    required this.homeId,
    required this.type,
    required this.title,
    required this.body,
    this.isRead = false,
    required this.createdAt,
    this.data,
  });

  factory NotificationItem.fromJson(Map<String, dynamic> json) {
    return NotificationItem(
      id: json['id'] as String,
      homeId: json['home_id'] as String,
      type: _parseType(json['type'] as String?),
      title: json['title'] as String,
      body: json['body'] as String,
      isRead: json['is_read'] as bool? ?? false,
      createdAt: DateTime.parse(json['created_at'] as String),
      data: json['data'] as Map<String, dynamic>?,
    );
  }

  static NotificationType _parseType(String? type) {
    switch (type) {
      case 'device_approval_request':
        return NotificationType.deviceApprovalRequest;
      case 'quota_exceeded':
        return NotificationType.quotaExceeded;
      case 'new_device':
        return NotificationType.newDevice;
      case 'device_offline':
        return NotificationType.deviceOffline;
      case 'alert':
        return NotificationType.alert;
      default:
        return NotificationType.unknown;
    }
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'home_id': homeId,
      'type': type.name,
      'title': title,
      'body': body,
      'is_read': isRead,
      'created_at': createdAt.toIso8601String(),
      'data': data,
    };
  }

  NotificationItem copyWith({bool? isRead}) {
    return NotificationItem(
      id: id,
      homeId: homeId,
      type: type,
      title: title,
      body: body,
      isRead: isRead ?? this.isRead,
      createdAt: createdAt,
      data: data,
    );
  }
}
