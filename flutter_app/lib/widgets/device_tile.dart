import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../models/network_device.dart';
import '../providers/home_provider.dart';

class DeviceTile extends StatelessWidget {
  final NetworkDevice device;

  const DeviceTile({super.key, required this.device});

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final home = context.watch<HomeProvider>();

    final statusColor = switch (device.approvalStatus) {
      ApprovalStatus.approved => Colors.green,
      ApprovalStatus.blocked => Colors.red,
      ApprovalStatus.pending => Colors.orange,
    };

    final statusLabel = switch (device.approvalStatus) {
      ApprovalStatus.approved => 'Approved',
      ApprovalStatus.blocked => 'Blocked',
      ApprovalStatus.pending => 'Pending',
    };

    return Dismissible(
      key: Key(device.id),
      direction: device.approvalStatus == ApprovalStatus.pending
          ? DismissDirection.horizontal
          : DismissDirection.none,
      background: Container(
        color: Colors.green,
        alignment: Alignment.centerLeft,
        padding: const EdgeInsets.only(left: 20),
        child: const Icon(Icons.check, color: Colors.white),
      ),
      secondaryBackground: Container(
        color: Colors.red,
        alignment: Alignment.centerRight,
        padding: const EdgeInsets.only(right: 20),
        child: const Icon(Icons.block, color: Colors.white),
      ),
      onDismissed: (direction) {
        if (direction == DismissDirection.startToEnd) {
          home.approveDevice(device.id);
        } else {
          home.blockDevice(device.id);
        }
      },
      child: Card(
        margin: const EdgeInsets.symmetric(horizontal: 12, vertical: 4),
        child: ListTile(
          leading: CircleAvatar(
            backgroundColor: device.isOnline
                ? Colors.green.withOpacity(0.2)
                : Colors.grey.withOpacity(0.2),
            child: Icon(
              device.isOnline ? Icons.wifi : Icons.wifi_off,
              color: device.isOnline ? Colors.green : Colors.grey,
            ),
          ),
          title: Text(
            device.hostname ?? 'Unknown Device',
            style: theme.textTheme.bodyMedium?.copyWith(
              fontWeight: FontWeight.w600,
            ),
          ),
          subtitle: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              if (device.macAddress != null)
                Text(
                  device.macAddress!,
                  style: theme.textTheme.bodySmall,
                ),
              if (device.ipAddress != null)
                Text(
                  device.ipAddress!,
                  style: theme.textTheme.bodySmall,
                ),
              if (device.assignedProfileName != null)
                Text(
                  'Assigned to: ${device.assignedProfileName}',
                  style: theme.textTheme.bodySmall?.copyWith(
                    color: theme.colorScheme.primary,
                  ),
                ),
            ],
          ),
          trailing: Row(
            mainAxisSize: MainAxisSize.min,
            children: [
              Chip(
                label: Text(
                  statusLabel,
                  style: TextStyle(
                    color: statusColor,
                    fontSize: 11,
                  ),
                ),
                backgroundColor: statusColor.withOpacity(0.1),
                side: BorderSide.none,
                padding: EdgeInsets.zero,
                visualDensity: VisualDensity.compact,
              ),
              PopupMenuButton<String>(
                onSelected: (value) async {
                  switch (value) {
                    case 'approve':
                      await home.approveDevice(device.id);
                    case 'block':
                      await home.blockDevice(device.id);
                    case 'assign':
                      _showAssignDialog(context, home);
                  }
                },
                itemBuilder: (context) => [
                  if (device.approvalStatus != ApprovalStatus.approved)
                    const PopupMenuItem(
                      value: 'approve',
                      child: ListTile(
                        leading: Icon(Icons.check, color: Colors.green),
                        title: Text('Approve'),
                        dense: true,
                      ),
                    ),
                  if (device.approvalStatus != ApprovalStatus.blocked)
                    const PopupMenuItem(
                      value: 'block',
                      child: ListTile(
                        leading: Icon(Icons.block, color: Colors.red),
                        title: Text('Block'),
                        dense: true,
                      ),
                    ),
                  const PopupMenuItem(
                    value: 'assign',
                    child: ListTile(
                      leading: Icon(Icons.person),
                      title: Text('Assign to Profile'),
                      dense: true,
                    ),
                  ),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }

  void _showAssignDialog(BuildContext context, HomeProvider home) {
    final profiles = home.profiles
        .where((p) => p.id != device.assignedProfileId)
        .toList();

    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Assign to Profile'),
        content: profiles.isEmpty
            ? const Text('No profiles available')
            : SizedBox(
                width: double.maxFinite,
                child: ListView.builder(
                  shrinkWrap: true,
                  itemCount: profiles.length,
                  itemBuilder: (context, index) {
                    final profile = profiles[index];
                    return ListTile(
                      leading: CircleAvatar(child: Text(profile.name.isNotEmpty ? profile.name[0] : '?')),
                      title: Text(profile.name),
                      onTap: () {
                        Navigator.pop(context);
                        home.assignDeviceToProfile(
                          device.id,
                          profile.id,
                          profile.name,
                        );
                      },
                    );
                  },
                ),
              ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: const Text('Cancel'),
          ),
        ],
      ),
    );
  }
}
