class QuotaRule {
  final String id;
  final String profileId;
  final double limitBytes;
  final String period;

  const QuotaRule({
    required this.id,
    required this.profileId,
    required this.limitBytes,
    this.period = 'monthly',
  });

  factory QuotaRule.fromJson(Map<String, dynamic> json) {
    return QuotaRule(
      id: json['id'] as String,
      profileId: json['profile_id'] as String,
      limitBytes: (json['limit_bytes'] as num).toDouble(),
      period: json['period'] as String? ?? 'monthly',
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'profile_id': profileId,
      'limit_bytes': limitBytes,
      'period': period,
    };
  }
}
