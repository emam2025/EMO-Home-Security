import 'package:flutter/material.dart';

class UsageBar extends StatelessWidget {
  final String label;
  final double value;
  final double maxValue;
  final String unit;
  final Color? barColor;

  const UsageBar({
    super.key,
    required this.label,
    required this.value,
    required this.maxValue,
    this.unit = 'GB',
    this.barColor,
  });

  double get _percentage => maxValue > 0 ? (value / maxValue * 100).clamp(0, 100) : 0.0;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final color = barColor ??
        (_percentage > 80
            ? Colors.red
            : _percentage > 50
                ? Colors.orange
                : Colors.green);

    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 6),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Expanded(
                child: Text(
                  label,
                  style: theme.textTheme.bodyMedium,
                  overflow: TextOverflow.ellipsis,
                ),
              ),
              const SizedBox(width: 8),
              Text(
                '${value.toStringAsFixed(1)} / ${maxValue.toStringAsFixed(1)} $unit',
                style: theme.textTheme.bodySmall?.copyWith(
                  color: theme.colorScheme.onSurfaceVariant,
                ),
              ),
            ],
          ),
          const SizedBox(height: 4),
          ClipRRect(
            borderRadius: BorderRadius.circular(4),
            child: LinearProgressIndicator(
              value: _percentage / 100,
              minHeight: 8,
              backgroundColor: theme.colorScheme.surfaceContainerHighest,
              color: color,
            ),
          ),
          Text(
            '${_percentage.toStringAsFixed(0)}% used',
            style: theme.textTheme.bodySmall?.copyWith(
              color: theme.colorScheme.onSurfaceVariant,
            ),
          ),
        ],
      ),
    );
  }
}
