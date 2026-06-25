class TimeSlot {
  final int startHour;
  final int startMinute;
  final int endHour;
  final int endMinute;

  const TimeSlot({
    required this.startHour,
    required this.startMinute,
    required this.endHour,
    required this.endMinute,
  });

  factory TimeSlot.fromJson(Map<String, dynamic> json) {
    return TimeSlot(
      startHour: json['start_hour'] as int,
      startMinute: json['start_minute'] as int,
      endHour: json['end_hour'] as int,
      endMinute: json['end_minute'] as int,
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'start_hour': startHour,
      'start_minute': startMinute,
      'end_hour': endHour,
      'end_minute': endMinute,
    };
  }
}

class Schedule {
  final String id;
  final String profileId;
  final bool enabled;
  final List<int> activeDays;
  final List<TimeSlot> timeSlots;

  const Schedule({
    required this.id,
    required this.profileId,
    this.enabled = true,
    this.activeDays = const [],
    this.timeSlots = const [],
  });

  factory Schedule.fromJson(Map<String, dynamic> json) {
    return Schedule(
      id: json['id'] as String,
      profileId: json['profile_id'] as String,
      enabled: json['enabled'] as bool? ?? true,
      activeDays: (json['active_days'] as List<dynamic>?)
              ?.map((e) => e as int)
              .toList() ??
          [],
      timeSlots: (json['time_slots'] as List<dynamic>?)
              ?.map((e) => TimeSlot.fromJson(e as Map<String, dynamic>))
              .toList() ??
          [],
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'profile_id': profileId,
      'enabled': enabled,
      'active_days': activeDays,
      'time_slots': timeSlots.map((e) => e.toJson()).toList(),
    };
  }
}
