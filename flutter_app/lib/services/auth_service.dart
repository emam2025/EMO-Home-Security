import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import '../models/user.dart';
import 'api_client.dart';

class AuthService {
  final ApiClient _api = ApiClient();
  final FlutterSecureStorage _storage = const FlutterSecureStorage();

  Future<User> login(String email, String password) async {
    final response = await _api.login(email, password);
    final accessToken = response['access_token'] as String;
    final refreshToken = response['refresh_token'] as String;
    await _storage.write(key: 'access_token', value: accessToken);
    await _storage.write(key: 'refresh_token', value: refreshToken);
    return User.fromJson(response['user'] as Map<String, dynamic>);
  }

  Future<User> register(String email, String password, String name) async {
    final response = await _api.register(email, password, name);
    final accessToken = response['access_token'] as String;
    final refreshToken = response['refresh_token'] as String;
    await _storage.write(key: 'access_token', value: accessToken);
    await _storage.write(key: 'refresh_token', value: refreshToken);
    return User.fromJson(response['user'] as Map<String, dynamic>);
  }

  Future<bool> isAuthenticated() async {
    final token = await _storage.read(key: 'access_token');
    return token != null;
  }

  Future<void> logout() async {
    await _storage.delete(key: 'access_token');
    await _storage.delete(key: 'refresh_token');
  }
}
