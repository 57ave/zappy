/*
** EPITECH PROJECT, 2025
** Tile
** File description:
** Tile
*/

#ifndef TILE_HPP
    #define TILE_HPP
    #include <array>
    #include <vector>
    #include <algorithm>

class Tile {
    public:
        void setResources(const std::array<int, 7> &resources);
        const std::array<int, 7> &getResources() const;
        void addPlayer(int id);
        void removePlayer(int id);
        const std::vector<int> &getPlayers() const;
    private:
        std::array<int, 7> _resources{};
        std::vector<int> _playerIds;
};

#endif /* !TILE_HPP */