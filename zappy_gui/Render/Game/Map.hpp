/*
** EPITECH PROJECT, 2025
** MAP
** File description:
** Map
*/

#ifndef MAP_HPP
    #define MAP_HPP
    #include "Tile.hpp"
    #include <vector>

class Map {
    public:
        void resize(int width, int height);
        Tile &at(int x, int y);
        int getWidth() const;
        int getHeight() const;
    private:
        int _width;
        int _height;
        std::vector<std::vector<Tile>> _tiles;
};

#endif /* !MAP_HPP */