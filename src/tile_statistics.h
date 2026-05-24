#ifndef TILE_STATISTICS_H
#define TILE_STATISTICS_H

#include <vector>
#include <chrono>
#include <algorithm>
#include <iostream>

struct TileStats {
    int tile_index;
    double render_time_ms;
    int pixel_count;
    double pixels_per_ms;
};

class TileStatisticsCollector {
private:
    std::vector<TileStats> stats;
    bool enabled;

public:
    TileStatisticsCollector(bool enable = false) : enabled(enable) {}

    void record_tile(int tile_index, double render_time_ms, int pixel_count) {
        if (!enabled) return;

        TileStats stat;
        stat.tile_index = tile_index;
        stat.render_time_ms = render_time_ms;
        stat.pixel_count = pixel_count;
        stat.pixels_per_ms = (render_time_ms > 0) ? pixel_count / render_time_ms : 0;

        stats.push_back(stat);
    }

    void print_report() const {
        if (!enabled || stats.empty()) return;

        std::clog << "\n=== Tile Rendering Statistics ===\n";

        double total_time = 0;
        int total_pixels = 0;
        double min_time = stats[0].render_time_ms;
        double max_time = stats[0].render_time_ms;

        for (const auto& stat : stats) {
            total_time += stat.render_time_ms;
            total_pixels += stat.pixel_count;
            min_time = std::min(min_time, stat.render_time_ms);
            max_time = std::max(max_time, stat.render_time_ms);
        }

        double avg_time = total_time / stats.size();

        std::clog << "Total tiles: " << stats.size() << "\n";
        std::clog << "Total render time: " << total_time << " ms\n";
        std::clog << "Total pixels: " << total_pixels << "\n";
        std::clog << "Average time per tile: " << avg_time << " ms\n";
        std::clog << "Min time per tile: " << min_time << " ms\n";
        std::clog << "Max time per tile: " << max_time << " ms\n";
        std::clog << "Tile time variance: " << (max_time - min_time) << " ms\n";
        std::clog << "Overall throughput: " << (total_pixels / total_time) << " pixels/ms\n";
        std::clog << "================================\n\n";
    }

    void enable() { enabled = true; }
    void disable() { enabled = false; }
    bool is_enabled() const { return enabled; }

    const std::vector<TileStats>& get_stats() const { return stats; }
    void clear() { stats.clear(); }
};

#endif
