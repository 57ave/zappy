/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Tile.cpp
*/

#include "../include/Tile.hpp"

void Tile::setResources(const std::array<int, 7> &resources) {
    _resources = resources;
}

const std::array<int, 7> &Tile::getResources() const {
    return _resources;
}

void Tile::addPlayer(int id) {
    _playerIds.push_back(id);
}

void Tile::removePlayer(int id) {

}

const std::vector<int> &Tile::getPlayers() const {
    return _playerIds;
}