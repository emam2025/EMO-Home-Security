import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/auth_provider.dart';
import '../providers/home_provider.dart';
import '../widgets/profile_card.dart';

class ProfilesScreen extends StatefulWidget {
  const ProfilesScreen({super.key});

  @override
  State<ProfilesScreen> createState() => _ProfilesScreenState();
}

class _ProfilesScreenState extends State<ProfilesScreen> {
  Future<void> _addProfile() async {
    final nameController = TextEditingController();
    final name = await showDialog<String>(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('New Profile'),
        content: TextField(
          controller: nameController,
          decoration: const InputDecoration(
            labelText: 'Profile Name',
            border: OutlineInputBorder(),
          ),
          autofocus: true,
          textCapitalization: TextCapitalization.words,
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: const Text('Cancel'),
          ),
          FilledButton(
            onPressed: () => Navigator.pop(context, nameController.text.trim()),
            child: const Text('Create'),
          ),
        ],
      ),
    );

    if (name != null && name.isNotEmpty && mounted) {
      await context.read<HomeProvider>().createProfile(name);
    }
  }

  @override
  Widget build(BuildContext context) {
    final home = context.watch<HomeProvider>();

    return Scaffold(
      body: RefreshIndicator(
        onRefresh: () async {
          final auth = context.read<AuthProvider>();
          if (auth.currentUser?.homeId != null) {
            await home.loadHome(auth.currentUser!.homeId!);
          }
        },
        child: home.profiles.isEmpty
            ? const Center(child: Text('No profiles yet'))
            : ListView.builder(
                padding: const EdgeInsets.all(8),
                itemCount: home.profiles.length,
                itemBuilder: (context, index) {
                  final profile = home.profiles[index];
                  return ProfileCard(
                    profile: profile,
                    onTap: () {
                      Navigator.pushNamed(
                        context,
                        '/profile_detail',
                        arguments: profile,
                      );
                    },
                  );
                },
              ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _addProfile,
        child: const Icon(Icons.add),
      ),
    );
  }
}
