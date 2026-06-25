enum ApprovalStatus { pending, approved, blocked }

class NetworkDevice {
  final String id;
  final String homeId;
  final String? macAddress;
  final String? ipAddress;
  final String? hostname;
  final bool isOnline;
  final ApprovalStatus approvalStatus;
  final String? assignedProfileId;
  final String? assignedProfileName;
  final DateTime? firstSeen;

  const NetworkDevice({
    required this.id,
    required this.homeId,
    this.macAddress,
    this.ipAddress,
    this.hostname,
    this.isOnline = false,
    this.approvalStatus = ApprovalStatus.pending,
    this.assignedProfileId,
    this.assignedProfileName,
    this.firstSeen,
  });

  factory NetworkDevice.fromJson(Map<String, dynamic> json) {
    return NetworkDevice(
      id: json['id'] as String,
      homeId: json['home_id'] as String,
      macAddress: json['mac_address'] as String?,
      ipAddress: json['ip_address'] as String?,
      hostname: json['hostname'] as String?,
      isOnline: json['is_online'] as bool? ?? false,
      approvalStatus: _parseApprovalStatus(json['approval_status'] as String?),
      assignedProfileId: json['assigned_profile_id'] as String?,
      assignedProfileName: json['assigned_profile_name'] as String?,
      firstSeen: json['first_seen'] != null
          ? DateTime.parse(json['first_seen'] as String)
          : null,
    );
  }

  static ApprovalStatus _parseApprovalStatus(String? status) {
    switch (status) {
      case 'approved':
        return ApprovalStatus.approved;
      case 'blocked':
        return ApprovalStatus.blocked;
      default:
        return ApprovalStatus.pending;
    }
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'home_id': homeId,
      'mac_address': macAddress,
      'ip_address': ipAddress,
      'hostname': hostname,
      'is_online': isOnline,
      'approval_status': approvalStatus.name,
      'assigned_profile_id': assignedProfileId,
      'assigned_profile_name': assignedProfileName,
      'first_seen': firstSeen?.toIso8601String(),
    };
  }

  NetworkDevice copyWith({
    ApprovalStatus? approvalStatus,
    String? assignedProfileId,
    String? assignedProfileName,
  }) {
    return NetworkDevice(
      id: id,
      homeId: homeId,
      macAddress: macAddress,
      ipAddress: ipAddress,
      hostname: hostname,
      isOnline: isOnline,
      approvalStatus: approvalStatus ?? this.approvalStatus,
      assignedProfileId: assignedProfileId ?? this.assignedProfileId,
      assignedProfileName: assignedProfileName ?? this.assignedProfileName,
      firstSeen: firstSeen,
    );
  }
}
