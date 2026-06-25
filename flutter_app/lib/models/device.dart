enum DeviceType { router, switch_, accessPoint, unknown }

enum DeviceStatus { online, offline, error }

class Device {
  final String id;
  final String homeId;
  final String name;
  final DeviceType type;
  final String? model;
  final String? firmwareVersion;
  final String? ipAddress;
  final DeviceStatus status;

  const Device({
    required this.id,
    required this.homeId,
    required this.name,
    required this.type,
    this.model,
    this.firmwareVersion,
    this.ipAddress,
    this.status = DeviceStatus.offline,
  });

  factory Device.fromJson(Map<String, dynamic> json) {
    return Device(
      id: json['id'] as String,
      homeId: json['home_id'] as String,
      name: json['name'] as String,
      type: _parseDeviceType(json['type'] as String?),
      model: json['model'] as String?,
      firmwareVersion: json['firmware_version'] as String?,
      ipAddress: json['ip_address'] as String?,
      status: _parseDeviceStatus(json['status'] as String?),
    );
  }

  static DeviceType _parseDeviceType(String? type) {
    switch (type) {
      case 'router':
        return DeviceType.router;
      case 'switch':
        return DeviceType.switch_;
      case 'access_point':
        return DeviceType.accessPoint;
      default:
        return DeviceType.unknown;
    }
  }

  static DeviceStatus _parseDeviceStatus(String? status) {
    switch (status) {
      case 'online':
        return DeviceStatus.online;
      case 'offline':
        return DeviceStatus.offline;
      case 'error':
        return DeviceStatus.error;
      default:
        return DeviceStatus.offline;
    }
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'home_id': homeId,
      'name': name,
      'type': type.name.replaceAll('_', ''),
      'model': model,
      'firmware_version': firmwareVersion,
      'ip_address': ipAddress,
      'status': status.name,
    };
  }
}
