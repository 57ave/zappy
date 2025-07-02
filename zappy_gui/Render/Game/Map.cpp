/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Map.cpp
*/

#include "Map.hpp"

void Map::resize(int width, int height) {
    _width = width;
    _height = height;
    _tiles.resize(height, std::vector<Tile>(width));
}

Tile &Map::at(int x, int y) {
    return _tiles[y][x];
}

int Map::getWidth() const {
    return _width;
}

int Map::getHeight() const {
    return _height;
}