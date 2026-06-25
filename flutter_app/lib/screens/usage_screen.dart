import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/home_provider.dart';

class UsageScreen extends StatefulWidget {
  const UsageScreen({super.key});

  @override
  State<UsageScreen> createState() => _UsageScreenState();
}

class _UsageScreenState extends State<UsageScreen> {
  String _period = 'current_month';

  @override
  Widget build(BuildContext context) {
    final home = context.watch<HomeProvider>();
    final theme = Theme.of(context);

    return ListView(
      padding: const EdgeInsets.all(16),
      children: [
        Row(
          children: [
            Text('Usage Overview', style: theme.textTheme.titleMedium),
            const Spacer(),
            SegmentedButton<String>(
              segments: const [
                ButtonSegment(value: 'current_month', label: Text('This Month')),
                ButtonSegment(value: 'last_month', label: Text('Last Month')),
              ],
              selected: {_period},
              onSelectionChanged: (selected) {
                setState(() => _period = selected.first);
              },
            ),
          ],
        ),
        const SizedBox(height: 24),
        SizedBox(
          height: 200,
          child: _buildBarChart(home, theme),
        ),
        const SizedBox(height: 24),
        Text('Per Profile', style: theme.textTheme.titleMedium),
        const SizedBox(height: 12),
        ...home.profiles.map((profile) {
          final consumedGb = profile.consumptionBytes / (1024 * 1024 * 1024);
          final quotaGb = profile.quotaBytes / (1024 * 1024 * 1024);
          final percent =
              quotaGb > 0 ? (consumedGb / quotaGb * 100).clamp(0, 100) : 0.0;

          return Padding(
            padding: const EdgeInsets.only(bottom: 12),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text(profile.name),
                    Text('${consumedGb.toStringAsFixed(1)} GB'),
                  ],
                ),
                const SizedBox(height: 4),
                ClipRRect(
                  borderRadius: BorderRadius.circular(4),
                  child: LinearProgressIndicator(
                    value: percent / 100,
                    minHeight: 8,
                    backgroundColor: theme.colorScheme.surfaceContainerHighest,
                    color: percent > 80
                        ? Colors.red
                        : percent > 50
                            ? Colors.orange
                            : Colors.green,
                  ),
                ),
                Text(
                  '${percent.toStringAsFixed(0)}% of ${quotaGb.toStringAsFixed(1)} GB',
                  style: theme.textTheme.bodySmall?.copyWith(
                    color: theme.colorScheme.onSurfaceVariant,
                  ),
                ),
              ],
            ),
          );
        }),
      ],
    );
  }

  Widget _buildBarChart(HomeProvider home, ThemeData theme) {
    if (home.profiles.isEmpty) {
      return const Center(child: Text('No usage data'));
    }

    final maxConsumption = home.profiles
        .map((p) => p.consumptionBytes)
        .reduce((a, b) => a > b ? a : b);

    return BarChart(
      BarChartData(
        alignment: BarChartAlignment.spaceAround,
        maxY: maxConsumption > 0 ? maxConsumption * 1.2 : 1,
        barGroups: home.profiles.asMap().entries.map((entry) {
          final i = entry.key;
          final profile = entry.value;
          return BarChartGroupData(
            x: i,
            barRods: [
              BarChartRodData(
                toY: profile.consumptionBytes > 0 ? profile.consumptionBytes : 0,
                color: Colors.indigo,
                width: 20,
                borderRadius: const BorderRadius.vertical(
                  top: Radius.circular(4),
                ),
              ),
            ],
          );
        }).toList(),
        titlesData: FlTitlesData(
          show: true,
          bottomTitles: AxisTitles(
            sideTitles: SideTitles(
              showTitles: true,
              getTitlesWidget: (value, meta) {
                final index = value.toInt();
                if (index >= 0 && index < home.profiles.length) {
                  return Padding(
                    padding: const EdgeInsets.only(top: 4),
                    child: Text(
                      home.profiles[index].name,
                      style: const TextStyle(fontSize: 10),
                    ),
                  );
                }
                return const Text('');
              },
            ),
          ),
          leftTitles: AxisTitles(
            sideTitles: SideTitles(
              showTitles: true,
              reservedSize: 40,
              getTitlesWidget: (value, meta) {
                final gb = value / (1024 * 1024 * 1024);
                return Text(
                  '${gb.toStringAsFixed(0)} GB',
                  style: const TextStyle(fontSize: 10),
                );
              },
            ),
          ),
          topTitles: const AxisTitles(
            sideTitles: SideTitles(showTitles: false),
          ),
          rightTitles: const AxisTitles(
            sideTitles: SideTitles(showTitles: false),
          ),
        ),
        gridData: FlGridData(
          show: true,
          drawVerticalLine: false,
          horizontalInterval: maxConsumption > 0 ? maxConsumption / 4 : 1,
        ),
        borderData: FlBorderData(show: false),
      ),
    );
  }
}
