#ifndef GRID_H
#define GRID_H

#include <algorithm>
#include <cmath>
#include <vector>

#include "physics/engine.h"
#include "physics/vec2d.h"

struct SpatialGrid {
    float cell_size;
    int cols;
    int rows;

    std::vector<std::vector<int>> cells;

    SpatialGrid(float screen_width, float screen_height, float cell_size) {
        this->cell_size = cell_size;
        this->cols = static_cast<int>(std::ceil(screen_width / cell_size));
        this->rows = static_cast<int>(std::ceil(screen_height / cell_size));
        cells.resize(cols * rows);
    }

    void build(const std::vector<Node>& nodes) noexcept {
        for (auto& cell : cells) {
            cell.clear();
        }

        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            int cx = std::clamp(static_cast<int>(nodes[i].pos.x / cell_size), 0, cols - 1);
            int cy = std::clamp(static_cast<int>(nodes[i].pos.y / cell_size), 0, rows - 1);
            int cell_idx = cy * cols + cx;
            cells[cell_idx].push_back(i);
        }
    }
};

#endif