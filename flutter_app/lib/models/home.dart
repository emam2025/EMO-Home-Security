class Home {
  final String id;
  final String name;
  final String? address;
  final bool isPaused;

  const Home({
    required this.id,
    required this.name,
    this.address,
    this.isPaused = false,
  });

  factory Home.fromJson(Map<String, dynamic> json) {
    return Home(
      id: json['id'] as String,
      name: json['name'] as String,
      address: json['address'] as String?,
      isPaused: json['is_paused'] as bool? ?? false,
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'name': name,
      'address': address,
      'is_paused': isPaused,
    };
  }

  Home copyWith({bool? isPaused}) {
    return Home(
      id: id,
      name: name,
      address: address,
      isPaused: isPaused ?? this.isPaused,
    );
  }
}
