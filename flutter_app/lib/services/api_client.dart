import 'dart:convert';
import 'dart:io';
import 'package:flutter/foundation.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:http/http.dart' as http;

class ApiException implements Exception {
  final int statusCode;
  final String message;

  const ApiException(this.statusCode, this.message);

  @override
  String toString() => 'ApiException($statusCode): $message';
}

class ApiClient {
  static final ApiClient _instance = ApiClient._internal();
  factory ApiClient() => _instance;

  String baseUrl = 'http://localhost:3000/v1';
  final FlutterSecureStorage _storage = const FlutterSecureStorage();
  http.Client _httpClient = http.Client();

  ApiClient._internal();

  void setBaseUrl(String url) {
    baseUrl = url;
  }

  void setHttpClient(http.Client client) {
    _httpClient = client;
  }

  Future<String?> _getToken() async {
    return await _storage.read(key: 'access_token');
  }

  Future<String?> _getRefreshToken() async {
    return await _storage.read(key: 'refresh_token');
  }

  Future<void> _saveTokens(String access, String refresh) async {
    await _storage.write(key: 'access_token', value: access);
    await _storage.write(key: 'refresh_token', value: refresh);
  }

  Future<Map<String, String>> _headers({bool withAuth = true}) async {
    final headers = <String, String>{
      'Content-Type': 'application/json',
      'Accept': 'application/json',
    };
    if (withAuth) {
      final token = await _getToken();
      if (token != null) {
        headers['Authorization'] = 'Bearer $token';
      }
    }
    return headers;
  }

  Future<dynamic> _refreshToken() async {
    final refreshToken = await _getRefreshToken();
    if (refreshToken == null) {
      throw const ApiException(401, 'No refresh token available');
    }
    final response = await _httpClient.post(
      Uri.parse('$baseUrl/auth/refresh'),
      headers: {'Content-Type': 'application/json'},
      body: jsonEncode({'refresh_token': refreshToken}),
    );
    if (response.statusCode == 200) {
      final data = jsonDecode(response.body) as Map<String, dynamic>;
      await _saveTokens(
        data['access_token'] as String,
        data['refresh_token'] as String,
      );
      return data;
    }
    throw ApiException(response.statusCode, 'Token refresh failed');
  }

  Future<dynamic> _request(
    String method,
    String path, {
    Map<String, dynamic>? body,
    Map<String, String>? queryParams,
    bool withAuth = true,
  }) async {
    final uri = Uri.parse('$baseUrl$path').replace(queryParameters: queryParams);
    final headers = await _headers(withAuth: withAuth);

    http.Response response;
    try {
      response = await _httpClient.send(
        http.Request(method, uri)..headers.addAll(headers)
          ..body = body != null ? jsonEncode(body) : '',
      ).then((res) => http.Response.fromStream(res));
    } on SocketException {
      throw const ApiException(0, 'Network error: Unable to reach server');
    }

    if (response.statusCode == 401 && withAuth) {
      try {
        await _refreshToken();
        final newHeaders = await _headers(withAuth: true);
        response = await _httpClient.send(
          http.Request(method, uri)..headers.addAll(newHeaders)
            ..body = body != null ? jsonEncode(body) : '',
        ).then((res) => http.Response.fromStream(res));
      } on ApiException {
        rethrow;
      }
    }

    if (response.statusCode >= 200 && response.statusCode < 300) {
      if (response.body.isEmpty) return null;
      return jsonDecode(response.body);
    }

    String message;
    try {
      final errorBody = jsonDecode(response.body) as Map<String, dynamic>;
      message = errorBody['message'] as String? ??
          errorBody['error'] as String? ??
          'Unknown error';
    } catch (_) {
      message = response.body.isNotEmpty ? response.body : 'Unknown error';
    }
    throw ApiException(response.statusCode, message);
  }

  Future<dynamic> get(String path, {Map<String, String>? queryParams}) =>
      _request('GET', path, queryParams: queryParams);

  Future<dynamic> post(String path, {Map<String, dynamic>? body}) =>
      _request('POST', path, body: body);

  Future<dynamic> put(String path, {Map<String, dynamic>? body}) =>
      _request('PUT', path, body: body);

  Future<dynamic> patch(String path, {Map<String, dynamic>? body}) =>
      _request('PATCH', path, body: body);

  Future<dynamic> delete(String path) => _request('DELETE', path);

  Future<dynamic> login(String email, String password) => post('/auth/login', body: {
        'email': email,
        'password': password,
      });

  Future<dynamic> register(String email, String password, String name) =>
      post('/auth/register', body: {
        'email': email,
        'password': password,
        'name': name,
      });

  Future<dynamic> refreshToken() => _refreshToken();
  Future<dynamic> getHome(String homeId) => get('/homes/$homeId');
  Future<dynamic> pauseHome(String homeId) => post('/homes/$homeId/pause');
  Future<dynamic> resumeHome(String homeId) => post('/homes/$homeId/resume');

  Future<dynamic> getProfiles(String homeId) => get('/homes/$homeId/profiles');
  Future<dynamic> createProfile(String homeId, Map<String, dynamic> data) =>
      post('/homes/$homeId/profiles', body: data);
  Future<dynamic> updateProfile(String homeId, String profileId, Map<String, dynamic> data) =>
      put('/homes/$homeId/profiles/$profileId', body: data);
  Future<dynamic> deleteProfile(String homeId, String profileId) =>
      delete('/homes/$homeId/profiles/$profileId');

  Future<dynamic> getDevices(String homeId) => get('/homes/$homeId/devices');
  Future<dynamic> getNetworkDevices(String homeId) =>
      get('/homes/$homeId/network-devices');
  Future<dynamic> approveNetworkDevice(String homeId, String deviceId) =>
      post('/homes/$homeId/network-devices/$deviceId/approve');
  Future<dynamic> blockNetworkDevice(String homeId, String deviceId) =>
      post('/homes/$homeId/network-devices/$deviceId/block');
  Future<dynamic> assignNetworkDevice(String homeId, String deviceId,
          Map<String, dynamic> data) =>
      post('/homes/$homeId/network-devices/$deviceId/assign', body: data);

  Future<dynamic> getQuotas(String homeId) => get('/homes/$homeId/quotas');
  Future<dynamic> updateQuota(String homeId, String profileId, Map<String, dynamic> data) =>
      put('/homes/$homeId/quotas/$profileId', body: data);

  Future<dynamic> getSchedules(String homeId) => get('/homes/$homeId/schedules');
  Future<dynamic> updateSchedule(String homeId, String profileId,
          Map<String, dynamic> data) =>
      put('/homes/$homeId/schedules/$profileId', body: data);

  Future<dynamic> getUsage(String homeId,
          {String? period, String? profileId}) =>
      get('/homes/$homeId/usage', queryParams: {
        if (period != null) 'period': period,
        if (profileId != null) 'profile_id': profileId,
      });

  Future<dynamic> getNotifications(String homeId) =>
      get('/homes/$homeId/notifications');
  Future<dynamic> markNotificationRead(String homeId, String notificationId) =>
      post('/homes/$homeId/notifications/$notificationId/read');
  Future<dynamic> getAlerts(String homeId) => get('/homes/$homeId/alerts');
}
