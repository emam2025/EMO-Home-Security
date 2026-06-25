class Profile {
  final String id;
  final String homeId;
  final String name;
  final String? avatarUrl;
  final bool isOnline;
  final double consumptionBytes;
  final double quotaBytes;

  const Profile({
    required this.id,
    required this.homeId,
    required this.name,
    this.avatarUrl,
    this.isOnline = false,
    this.consumptionBytes = 0,
    this.quotaBytes = 0,
  });

  factory Profile.fromJson(Map<String, dynamic> json) {
    return Profile(
      id: json['id'] as String,
      homeId: json['home_id'] as String,
      name: json['name'] as String,
      avatarUrl: json['avatar_url'] as String?,
      isOnline: json['is_online'] as bool? ?? false,
      consumptionBytes: (json['consumption_bytes'] as num?)?.toDouble() ?? 0,
      quotaBytes: (json['quota_bytes'] as num?)?.toDouble() ?? 0,
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'home_id': homeId,
      'name': name,
      'avatar_url': avatarUrl,
      'is_online': isOnline,
      'consumption_bytes': consumptionBytes,
      'quota_bytes': quotaBytes,
    };
  }

  Profile copyWith({
    String? name,
    String? avatarUrl,
    bool? isOnline,
    double? consumptionBytes,
    double? quotaBytes,
  }) {
    return Profile(
      id: id,
      homeId: homeId,
      name: name ?? this.name,
      avatarUrl: avatarUrl ?? this.avatarUrl,
      isOnline: isOnline ?? this.isOnline,
      consumptionBytes: consumptionBytes ?? this.consumptionBytes,
      quotaBytes: quotaBytes ?? this.quotaBytes,
    );
  }
}
