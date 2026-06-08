#ifndef GRID_H
#define GRID_H

#include <algorithm>
#include <cmath>
#include <vector>

#include "physics/engine.h"
#include "physics/vec2d.h"

struct SpatialGrid {
    float cell_size;
    int table_size;
    
    std::vector<int> counts;
    std::vector<int> starts;
    std::vector<int> entries;

    SpatialGrid(float /*dummy_w*/, float /*dummy_h*/, float cell_size) {
        this->cell_size = cell_size;
        this->table_size = 4093; // Prime number
        counts.resize(table_size, 0);
        starts.resize(table_size, 0);
    }

    inline unsigned int get_hash(int cx, int cy) const noexcept {
        return (static_cast<unsigned int>(cx * 73856093) ^ static_cast<unsigned int>(cy * 19349663)) % table_size;
    }

    void build(const std::vector<Node>& nodes) noexcept {
        int n = static_cast<int>(nodes.size());
        if (entries.size() < static_cast<size_t>(n)) {
            entries.resize(n);
        }

        std::fill(counts.begin(), counts.end(), 0);
        std::vector<unsigned int> hashes(n);

        for (int i = 0; i < n; ++i) {
            int cx = static_cast<int>(std::floor(nodes[i].pos.x / cell_size));
            int cy = static_cast<int>(std::floor(nodes[i].pos.y / cell_size));
            unsigned int hash = get_hash(cx, cy);
            hashes[i] = hash;
            counts[hash]++;
        }

        int current_start = 0;
        for (int i = 0; i < table_size; ++i) {
            starts[i] = current_start;
            current_start += counts[i];
            counts[i] = 0;
        }

        for (int i = 0; i < n; ++i) {
            unsigned int hash = hashes[i];
            int offset = starts[hash] + counts[hash];
            entries[offset] = i;
            counts[hash]++;
        }
    }
};

#endif