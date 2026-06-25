import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../models/notification_item.dart';
import '../providers/auth_provider.dart';
import '../providers/home_provider.dart';

class NotificationsScreen extends StatelessWidget {
  const NotificationsScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final home = context.watch<HomeProvider>();
    final theme = Theme.of(context);

    return Scaffold(
      appBar: AppBar(
        title: const Text('Notifications'),
        actions: [
          if (home.notifications.any((n) => !n.isRead))
            TextButton(
              onPressed: () => home.markAllNotificationsRead(),
              child: const Text('Mark all read'),
            ),
        ],
      ),
      body: home.notifications.isEmpty
          ? Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Icon(Icons.notifications_none, size: 64, color: theme.colorScheme.outline),
                  const SizedBox(height: 16),
                  Text('No notifications', style: theme.textTheme.titleMedium),
                ],
              ),
            )
          : RefreshIndicator(
              onRefresh: () async {
                final auth = context.read<AuthProvider>();
                if (auth.currentUser?.homeId != null) {
                  await home.loadHome(auth.currentUser!.homeId!);
                }
              },
              child: ListView.separated(
                padding: const EdgeInsets.symmetric(vertical: 8),
                itemCount: home.notifications.length,
                separatorBuilder: (_, __) => const Divider(height: 1),
                itemBuilder: (context, index) {
                  final notification = home.notifications[index];
                  return _NotificationTile(notification: notification);
                },
              ),
            ),
    );
  }
}

class _NotificationTile extends StatelessWidget {
  final NotificationItem notification;

  const _NotificationTile({required this.notification});

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final iconData = _iconForType(notification.type);
    final color = _colorForType(notification.type);

    return ListTile(
      leading: CircleAvatar(
        backgroundColor: color.withValues(alpha: 0.15),
        child: Icon(iconData, color: color, size: 20),
      ),
      title: Text(
        notification.title,
        style: theme.textTheme.bodyMedium?.copyWith(
          fontWeight: notification.isRead ? FontWeight.normal : FontWeight.bold,
        ),
      ),
      subtitle: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          if (notification.body.isNotEmpty)
            Text(
              notification.body,
              style: theme.textTheme.bodySmall,
              maxLines: 2,
              overflow: TextOverflow.ellipsis,
            ),
          const SizedBox(height: 4),
          Text(
            _formatTime(notification.createdAt),
            style: theme.textTheme.labelSmall?.copyWith(
              color: theme.colorScheme.outline,
            ),
          ),
        ],
      ),
      trailing: notification.isRead
          ? null
          : Container(
              width: 10,
              height: 10,
              decoration: BoxDecoration(
                color: theme.colorScheme.primary,
                shape: BoxShape.circle,
              ),
            ),
      onTap: () {
        if (!notification.isRead) {
          context.read<HomeProvider>().markNotificationRead(notification.id);
        }
      },
    );
  }

  IconData _iconForType(NotificationType type) {
    switch (type) {
      case NotificationType.deviceApprovalRequest:
        return Icons.person_add;
      case NotificationType.quotaExceeded:
        return Icons.data_usage;
      case NotificationType.newDevice:
        return Icons.devices;
      case NotificationType.deviceOffline:
        return Icons.wifi_off;
      case NotificationType.alert:
        return Icons.warning;
      case NotificationType.unknown:
        return Icons.notifications;
    }
  }

  Color _colorForType(NotificationType type) {
    switch (type) {
      case NotificationType.deviceApprovalRequest:
        return Colors.blue;
      case NotificationType.quotaExceeded:
        return Colors.orange;
      case NotificationType.newDevice:
        return Colors.green;
      case NotificationType.deviceOffline:
        return Colors.red;
      case NotificationType.alert:
        return Colors.amber;
      case NotificationType.unknown:
        return Colors.grey;
    }
  }

  String _formatTime(DateTime time) {
    final now = DateTime.now();
    final diff = now.difference(time);
    if (diff.inMinutes < 1) return 'Just now';
    if (diff.inMinutes < 60) return '${diff.inMinutes}m ago';
    if (diff.inHours < 24) return '${diff.inHours}h ago';
    if (diff.inDays < 7) return '${diff.inDays}d ago';
    return '${time.month}/${time.day}/${time.year}';
  }
}
