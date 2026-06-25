import 'dart:convert';
import 'package:flutter/foundation.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class WebSocketService {
  WebSocketChannel? _channel;
  final FlutterSecureStorage _storage = const FlutterSecureStorage();
  String _baseUrl = 'ws://localhost:3000/ws';

  final ValueNotifier<bool> isConnected = ValueNotifier(false);

  VoidCallback? onDeviceStatusChange;
  VoidCallback? onNewDevice;
  VoidCallback? onAlert;
  VoidCallback? onProfileChange;

  void setBaseUrl(String url) {
    _baseUrl = url;
  }

  Future<void> connect(String homeId) async {
    final token = await _storage.read(key: 'access_token');
    if (token == null) return;

    try {
      final uri = Uri.parse('$_baseUrl/homes/$homeId?token=$token');
      _channel = WebSocketChannel.connect(uri);

      isConnected.value = true;

      _channel!.stream.listen(
        (message) {
          _handleMessage(jsonDecode(message as String) as Map<String, dynamic>);
        },
        onDone: () {
          isConnected.value = false;
        },
        onError: (_) {
          isConnected.value = false;
        },
      );
    } catch (_) {
      isConnected.value = false;
    }
  }

  void _handleMessage(Map<String, dynamic> message) {
    final type = message['type'] as String?;
    switch (type) {
      case 'device_status_change':
        onDeviceStatusChange?.call();
      case 'new_device':
        onNewDevice?.call();
      case 'alert':
        onAlert?.call();
      case 'profile_change':
        onProfileChange?.call();
    }
  }

  void disconnect() {
    _channel?.sink.close();
    _channel = null;
    isConnected.value = false;
  }

  bool get isConnectedValue => isConnected.value;
}
