import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../models/network_device.dart';
import '../providers/auth_provider.dart';
import '../providers/home_provider.dart';
import '../widgets/device_tile.dart';

class DevicesScreen extends StatefulWidget {
  const DevicesScreen({super.key});

  @override
  State<DevicesScreen> createState() => _DevicesScreenState();
}

class _DevicesScreenState extends State<DevicesScreen>
    with SingleTickerProviderStateMixin {
  late TabController _tabController;

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: 3, vsync: this);
  }

  @override
  void dispose() {
    _tabController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final home = context.watch<HomeProvider>();

    return Column(
      children: [
        TabBar(
          controller: _tabController,
          labelColor: Theme.of(context).colorScheme.primary,
          tabs: [
            Tab(text: 'All (${home.networkDevices.length})'),
            Tab(text: 'Pending (${home.devicesPending})'),
            Tab(
              text:
                  'Blocked (${home.networkDevices.where((d) => d.approvalStatus == ApprovalStatus.blocked).length})',
            ),
          ],
        ),
        Expanded(
          child: RefreshIndicator(
            onRefresh: () async {
              final auth = context.read<AuthProvider>();
              if (auth.currentUser?.homeId != null) {
                await home.loadHome(auth.currentUser!.homeId!);
              }
            },
            child: TabBarView(
              controller: _tabController,
              children: [
                _DeviceList(
                  devices: home.networkDevices,
                  filter: (_) => true,
                ),
                _DeviceList(
                  devices: home.networkDevices,
                  filter: (d) => d.approvalStatus == ApprovalStatus.pending,
                ),
                _DeviceList(
                  devices: home.networkDevices,
                  filter: (d) => d.approvalStatus == ApprovalStatus.blocked,
                ),
              ],
            ),
          ),
        ),
      ],
    );
  }
}

class _DeviceList extends StatelessWidget {
  final List<NetworkDevice> devices;
  final bool Function(NetworkDevice) filter;

  const _DeviceList({required this.devices, required this.filter});

  @override
  Widget build(BuildContext context) {
    final filtered = devices.where(filter).toList();

    if (filtered.isEmpty) {
      return const Center(child: Text('No devices found'));
    }

    return ListView.builder(
      padding: const EdgeInsets.symmetric(vertical: 8),
      itemCount: filtered.length,
      itemBuilder: (context, index) {
        return DeviceTile(device: filtered[index]);
      },
    );
  }
}
