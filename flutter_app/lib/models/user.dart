class User {
  final String id;
  final String email;
  final String name;
  final String? homeId;
  final String? role;

  const User({
    required this.id,
    required this.email,
    required this.name,
    this.homeId,
    this.role,
  });

  factory User.fromJson(Map<String, dynamic> json) {
    return User(
      id: json['id'] as String,
      email: json['email'] as String,
      name: json['name'] as String,
      homeId: json['home_id'] as String?,
      role: json['role'] as String?,
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'email': email,
      'name': name,
      'home_id': homeId,
      'role': role,
    };
  }
}
