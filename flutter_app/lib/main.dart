import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'providers/auth_provider.dart';
import 'providers/home_provider.dart';
import 'screens/login_screen.dart';
import 'screens/dashboard_screen.dart';
import 'screens/profile_detail_screen.dart';
import 'screens/devices_screen.dart';
import 'screens/profiles_screen.dart';
import 'screens/usage_screen.dart';
import 'screens/notifications_screen.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const EMOApp());
}

class EMOApp extends StatelessWidget {
  const EMOApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MultiProvider(
      providers: [
        ChangeNotifierProvider(create: (_) => AuthProvider()),
        ChangeNotifierProvider(create: (_) => HomeProvider()),
      ],
      child: MaterialApp(
        title: 'EMO Home Security',
        debugShowCheckedModeBanner: false,
        theme: ThemeData(
          colorSchemeSeed: Colors.indigo,
          useMaterial3: true,
          brightness: Brightness.light,
        ),
        darkTheme: ThemeData(
          colorSchemeSeed: Colors.indigo,
          useMaterial3: true,
          brightness: Brightness.dark,
        ),
        initialRoute: '/login',
        routes: {
          '/login': (_) => const LoginScreen(),
          '/home': (_) => const DashboardScreen(),
          '/dashboard': (_) => const DashboardScreen(),
          '/profiles': (_) => const ProfilesScreen(),
          '/devices': (_) => const DevicesScreen(),
          '/usage': (_) => const UsageScreen(),
          '/profile_detail': (_) => const ProfileDetailScreen(),
          '/notifications': (_) => const NotificationsScreen(),
        },
      ),
    );
  }
}
