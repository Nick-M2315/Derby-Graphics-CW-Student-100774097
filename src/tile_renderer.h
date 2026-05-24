#ifndef TILE_RENDERER_H
#define TILE_RENDERER_H

#include "vec3.h"
#include "color.h"
#include "hittable.h"
#include "ray.h"
#include <vector>
#include <omp.h>

struct Tile {
    int x_start;
    int y_start;
    int x_end;
    int y_end;
    int width;
    int height;
};

class TileRenderer {
private:
    int tile_size;
    int image_width;
    int image_height;
    std::vector<Tile> tiles;

    void generate_tiles() {
        tiles.clear();
        for (int y = 0; y < image_height; y += tile_size) {
            for (int x = 0; x < image_width; x += tile_size) {
                Tile tile;
                tile.x_start = x;
                tile.y_start = y;
                tile.x_end = std::min(x + tile_size, image_width);
                tile.y_end = std::min(y + tile_size, image_height);
                tile.width = tile.x_end - tile.x_start;
                tile.height = tile.y_end - tile.y_start;
                tiles.push_back(tile);
            }
        }
    }

public:
    TileRenderer(int width, int height, int size = 32) 
        : image_width(width), image_height(height), tile_size(size) {
        generate_tiles();
    }

    int get_tile_count() const {
        return tiles.size();
    }

    const Tile& get_tile(int index) const {
        return tiles[index];
    }

    std::vector<Tile>& get_all_tiles() {
        return tiles;
    }

    void set_tile_size(int size) {
        tile_size = size;
        generate_tiles();
    }

    int get_tile_size() const {
        return tile_size;
    }
};

#endif
