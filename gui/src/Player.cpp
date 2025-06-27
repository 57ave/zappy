/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Player.cpp
*/

#include "../include/Player.hpp"

Player::Player(int id, const std::string &team)
    : _id(id), _x(0), _y(0), _direction(0), _lvl(1), _team(team) {
    _inventory.fill(0);
}

void Player::setPosition(int x, int y, int dir) {
    _x = x;
    _y = y;
    _direction = dir;
}

void Player::setLevel(int lvl) {
    _lvl = lvl;
}

void Player::setInventory(const std::array<int, 7> &inventory) {
    _inventory = inventory;
}

int Player::getId() const{
    return _id;
}

int Player::getX() const {
    return _x;
}

int Player::getY() const {
    return _y;
}

int Player::getDirection() const {
    return _direction;
}

int Player::getLevel() const {
    return _lvl;
}

const std::string& Player::getTeam() const {
    return _team;
}

const std::array<int, 7> &Player::getInventory() const {
    return _inventory;
}