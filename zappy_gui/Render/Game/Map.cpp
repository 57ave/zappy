/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Map.cpp
*/

#include "Map.hpp"
#include <stdexcept>

void Map::resize(int width, int height) {
    _width = width;
    _height = height;
    _tiles.clear();
    _tiles.resize(height, std::vector<Tile>(width));
}

Tile &Map::at(int x, int y) {
    if (_tiles.empty() || y < 0 || y >= (int)_tiles.size() || x < 0 || x >= (int)_tiles[y].size()) {
        throw std::out_of_range("Map::at: invalid access");
    }
    return _tiles[y][x];
}

int Map::getWidth() const {
    return _width;
}

int Map::getHeight() const {
    return _height;
}