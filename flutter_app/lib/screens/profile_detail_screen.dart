import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../models/profile.dart';
import '../providers/home_provider.dart';

class ProfileDetailScreen extends StatefulWidget {
  const ProfileDetailScreen({super.key});

  @override
  State<ProfileDetailScreen> createState() => _ProfileDetailScreenState();
}

class _ProfileDetailScreenState extends State<ProfileDetailScreen> {
  late TextEditingController _nameController;
  Profile? _profile;
  bool _isSaving = false;

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    final args = ModalRoute.of(context)?.settings.arguments;
    if (args is Profile && _profile == null) {
      _profile = args;
      _nameController = TextEditingController(text: _profile!.name);
    }
  }

  @override
  void dispose() {
    _nameController.dispose();
    super.dispose();
  }

  Future<void> _save() async {
    if (_profile == null || _nameController.text.trim().isEmpty) return;

    setState(() => _isSaving = true);

    final home = context.read<HomeProvider>();
    final success = await home.updateProfile(
      _profile!.id,
      {'name': _nameController.text.trim()},
    );

    if (!mounted) return;
    setState(() => _isSaving = false);

    if (success) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Profile updated')),
      );
      Navigator.pop(context);
    } else if (home.error != null) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(home.error!)),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    if (_profile == null) {
      return const Scaffold(body: Center(child: Text('No profile selected')));
    }

    final theme = Theme.of(context);
    final home = context.watch<HomeProvider>();
    final quota = home.quotas.where((q) => q.profileId == _profile!.id).firstOrNull;
    final schedule =
        home.schedules.where((s) => s.profileId == _profile!.id).firstOrNull;
    final assignedDevices = home.networkDevices
        .where((d) => d.assignedProfileId == _profile!.id)
        .toList();

    final consumedGb = _profile!.consumptionBytes / (1024 * 1024 * 1024);
    final quotaGb = _profile!.quotaBytes / (1024 * 1024 * 1024);
    final usagePercent = quotaGb > 0 ? (consumedGb / quotaGb * 100).clamp(0, 100) : 0.0;

    return Scaffold(
      appBar: AppBar(
        title: Text(_profile!.name),
        actions: [
          IconButton(
            icon: _isSaving
                ? const SizedBox(
                    width: 20,
                    height: 20,
                    child: CircularProgressIndicator(strokeWidth: 2),
                  )
                : const Icon(Icons.save),
            onPressed: _isSaving ? null : _save,
          ),
        ],
      ),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          TextFormField(
            controller: _nameController,
            decoration: const InputDecoration(
              labelText: 'Profile Name',
              border: OutlineInputBorder(),
              prefixIcon: Icon(Icons.person),
            ),
          ),
          const SizedBox(height: 24),
          if (quota != null) ...[
            Text('Data Quota', style: theme.textTheme.titleMedium),
            const SizedBox(height: 8),
            LinearProgressIndicator(
              value: usagePercent / 100,
              minHeight: 8,
              borderRadius: BorderRadius.circular(4),
            ),
            const SizedBox(height: 4),
            Text(
              '${consumedGb.toStringAsFixed(1)} GB / ${quotaGb.toStringAsFixed(1)} GB (${usagePercent.toStringAsFixed(0)}%)',
              style: theme.textTheme.bodySmall,
            ),
            const SizedBox(height: 16),
          ],
          if (schedule != null) ...[
            Text('Schedule', style: theme.textTheme.titleMedium),
            const SizedBox(height: 8),
            Card(
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Row(
                      children: [
                        Icon(
                          schedule.enabled
                              ? Icons.check_circle
                              : Icons.cancel,
                          color: schedule.enabled ? Colors.green : Colors.red,
                        ),
                        const SizedBox(width: 8),
                        Text(
                          schedule.enabled ? 'Schedule Active' : 'Schedule Disabled',
                        ),
                      ],
                    ),
                    if (schedule.timeSlots.isNotEmpty) ...[
                      const SizedBox(height: 8),
                      ...schedule.timeSlots.map(
                        (slot) => Text(
                          '${slot.startHour.toString().padLeft(2, '0')}:${slot.startMinute.toString().padLeft(2, '0')} - '
                          '${slot.endHour.toString().padLeft(2, '0')}:${slot.endMinute.toString().padLeft(2, '0')}',
                        ),
                      ),
                    ],
                  ],
                ),
              ),
            ),
            const SizedBox(height: 16),
          ],
          Text('Assigned Devices', style: theme.textTheme.titleMedium),
          const SizedBox(height: 8),
          if (assignedDevices.isEmpty)
            const Card(
              child: Padding(
                padding: EdgeInsets.all(16),
                child: Text('No devices assigned'),
              ),
            )
          else
            ...assignedDevices.map(
              (device) => Card(
                child: ListTile(
                  title: Text(device.hostname ?? 'Unknown'),
                  subtitle: Text(device.macAddress ?? ''),
                  trailing: IconButton(
                    icon: const Icon(Icons.remove_circle_outline),
                    onPressed: () async {
                      await home.assignDeviceToProfile(
                        device.id,
                        '',
                        '',
                      );
                    },
                  ),
                ),
              ),
            ),
        ],
      ),
    );
  }
}
