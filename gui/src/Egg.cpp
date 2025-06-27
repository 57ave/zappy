/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Egg.cpp
*/

#include "../include/Egg.hpp"

Egg::Egg(int id, int playerId, int x, int y)
    : _id(id), _playerId(playerId), _x(x), _y(y) {}

int Egg::getId() const {
    return _id;
}

int Egg::getPlayerId() const {
    return _playerId;
}

int Egg::getX() const {
    return _x;
}

int Egg::getY() const {
    return _y;
}
