import 'package:flutter/foundation.dart';
import '../models/home.dart';
import '../models/profile.dart';
import '../models/device.dart';
import '../models/network_device.dart';
import '../models/quota_rule.dart';
import '../models/schedule.dart';
import '../models/notification_item.dart';
import '../services/api_client.dart';
import '../services/websocket_service.dart';

class HomeProvider extends ChangeNotifier {
  final ApiClient _api = ApiClient();
  final WebSocketService _ws = WebSocketService();

  Home? _home;
  List<Profile> _profiles = [];
  List<Device> _devices = [];
  List<NetworkDevice> _networkDevices = [];
  List<QuotaRule> _quotas = [];
  List<Schedule> _schedules = [];
  List<NotificationItem> _notifications = [];
  bool _isLoading = false;
  String? _error;

  Home? get home => _home;
  List<Profile> get profiles => _profiles;
  List<Device> get devices => _devices;
  List<NetworkDevice> get networkDevices => _networkDevices;
  List<QuotaRule> get quotas => _quotas;
  List<Schedule> get schedules => _schedules;
  List<NotificationItem> get notifications => _notifications;
  bool get isLoading => _isLoading;
  String? get error => _error;

  int get devicesOnline =>
      _networkDevices.where((d) => d.isOnline).length;
  int get devicesPending =>
      _networkDevices.where((d) => d.approvalStatus == ApprovalStatus.pending).length;

  Future<void> loadHome(String homeId) async {
    _isLoading = true;
    _error = null;
    notifyListeners();

    try {
      final results = await Future.wait([
        _api.getHome(homeId),
        _api.getProfiles(homeId),
        _api.getDevices(homeId),
        _api.getNetworkDevices(homeId),
        _api.getQuotas(homeId),
        _api.getSchedules(homeId),
        _api.getNotifications(homeId),
      ]);

      _home = Home.fromJson(results[0] as Map<String, dynamic>);
      _profiles = (results[1] as List<dynamic>)
          .map((e) => Profile.fromJson(e as Map<String, dynamic>))
          .toList();
      _devices = (results[2] as List<dynamic>)
          .map((e) => Device.fromJson(e as Map<String, dynamic>))
          .toList();
      _networkDevices = (results[3] as List<dynamic>)
          .map((e) => NetworkDevice.fromJson(e as Map<String, dynamic>))
          .toList();
      _quotas = (results[4] as List<dynamic>)
          .map((e) => QuotaRule.fromJson(e as Map<String, dynamic>))
          .toList();
      _schedules = (results[5] as List<dynamic>)
          .map((e) => Schedule.fromJson(e as Map<String, dynamic>))
          .toList();
      _notifications = (results[6] as List<dynamic>)
          .map((e) => NotificationItem.fromJson(e as Map<String, dynamic>))
          .toList();

      _isLoading = false;
      notifyListeners();

      _connectWebSocket(homeId);
    } on ApiException catch (e) {
      _error = e.message;
      _isLoading = false;
      notifyListeners();
    }
  }

  void _connectWebSocket(String homeId) {
    _ws.onDeviceStatusChange = () => _reloadDevices(homeId);
    _ws.onNewDevice = () => _reloadNetworkDevices(homeId);
    _ws.onAlert = () => _reloadNotifications(homeId);
    _ws.onProfileChange = () => _reloadProfiles(homeId);
    _ws.connect(homeId);
  }

  Future<void> _reloadDevices(String homeId) async {
    try {
      final data = await _api.getDevices(homeId);
      _devices = (data as List<dynamic>)
          .map((e) => Device.fromJson(e as Map<String, dynamic>))
          .toList();
      notifyListeners();
    } catch (e) {
      _error = 'Failed to reload devices: $e';
      notifyListeners();
    }
  }

  Future<void> _reloadNetworkDevices(String homeId) async {
    try {
      final data = await _api.getNetworkDevices(homeId);
      _networkDevices = (data as List<dynamic>)
          .map((e) => NetworkDevice.fromJson(e as Map<String, dynamic>))
          .toList();
      notifyListeners();
    } catch (e) {
      _error = 'Failed to reload network devices: $e';
      notifyListeners();
    }
  }

  Future<void> _reloadNotifications(String homeId) async {
    try {
      final data = await _api.getNotifications(homeId);
      _notifications = (data as List<dynamic>)
          .map((e) => NotificationItem.fromJson(e as Map<String, dynamic>))
          .toList();
      notifyListeners();
    } catch (e) {
      _error = 'Failed to reload notifications: $e';
      notifyListeners();
    }
  }

  Future<void> _reloadProfiles(String homeId) async {
    try {
      final data = await _api.getProfiles(homeId);
      _profiles = (data as List<dynamic>)
          .map((e) => Profile.fromJson(e as Map<String, dynamic>))
          .toList();
      notifyListeners();
    } catch (e) {
      _error = 'Failed to reload profiles: $e';
      notifyListeners();
    }
  }

  void disconnectWebSocket() {
    _ws.disconnect();
  }

  Future<bool> togglePause() async {
    if (_home == null) return false;
    try {
      if (_home!.isPaused) {
        await _api.resumeHome(_home!.id);
        _home = _home!.copyWith(isPaused: false);
      } else {
        await _api.pauseHome(_home!.id);
        _home = _home!.copyWith(isPaused: true);
      }
      notifyListeners();
      return true;
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
      return false;
    }
  }

  Future<Profile?> createProfile(String name) async {
    if (_home == null) return null;
    try {
      final data = await _api.createProfile(_home!.id, {
        'name': name,
      });
      final profile = Profile.fromJson(data as Map<String, dynamic>);
      _profiles.add(profile);
      notifyListeners();
      return profile;
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
      return null;
    }
  }

  Future<bool> updateProfile(String profileId, Map<String, dynamic> data) async {
    if (_home == null) return false;
    try {
      final result = await _api.updateProfile(_home!.id, profileId, data);
      final updated = Profile.fromJson(result as Map<String, dynamic>);
      final index = _profiles.indexWhere((p) => p.id == profileId);
      if (index != -1) {
        _profiles[index] = updated;
        notifyListeners();
      }
      return true;
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
      return false;
    }
  }

  Future<bool> deleteProfile(String profileId) async {
    if (_home == null) return false;
    try {
      await _api.deleteProfile(_home!.id, profileId);
      _profiles.removeWhere((p) => p.id == profileId);
      notifyListeners();
      return true;
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
      return false;
    }
  }

  Future<bool> approveDevice(String deviceId) async {
    if (_home == null) return false;
    try {
      await _api.approveNetworkDevice(_home!.id, deviceId);
      final index = _networkDevices.indexWhere((d) => d.id == deviceId);
      if (index != -1) {
        _networkDevices[index] = _networkDevices[index].copyWith(
          approvalStatus: ApprovalStatus.approved,
        );
        notifyListeners();
      }
      return true;
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
      return false;
    }
  }

  Future<bool> blockDevice(String deviceId) async {
    if (_home == null) return false;
    try {
      await _api.blockNetworkDevice(_home!.id, deviceId);
      final index = _networkDevices.indexWhere((d) => d.id == deviceId);
      if (index != -1) {
        _networkDevices[index] = _networkDevices[index].copyWith(
          approvalStatus: ApprovalStatus.blocked,
        );
        notifyListeners();
      }
      return true;
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
      return false;
    }
  }

  Future<bool> assignDeviceToProfile(
      String deviceId, String profileId, String profileName) async {
    if (_home == null) return false;
    try {
      await _api.assignNetworkDevice(_home!.id, deviceId, {
        'profile_id': profileId,
      });
      final index = _networkDevices.indexWhere((d) => d.id == deviceId);
      if (index != -1) {
        _networkDevices[index] = _networkDevices[index].copyWith(
          assignedProfileId: profileId,
          assignedProfileName: profileName,
        );
        notifyListeners();
      }
      return true;
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
      return false;
    }
  }

  Future<void> markNotificationRead(String notificationId) async {
    if (_home == null) return;
    try {
      await _api.markNotificationRead(_home!.id, notificationId);
      final index = _notifications.indexWhere((n) => n.id == notificationId);
      if (index != -1) {
        _notifications[index] = _notifications[index].copyWith(isRead: true);
        notifyListeners();
      }
    } catch (e) {
      _error = 'Failed to mark notification as read: $e';
      notifyListeners();
    }
  }

  Future<void> markAllNotificationsRead() async {
    for (final n in _notifications.where((n) => !n.isRead)) {
      await markNotificationRead(n.id);
    }
  }

  void clearError() {
    _error = null;
    notifyListeners();
  }

  @override
  void dispose() {
    _ws.disconnect();
    super.dispose();
  }
}
