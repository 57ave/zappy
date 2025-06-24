/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Player.hpp
*/

#ifndef PLAYER_HPP
    #define PLAYER_HPP
    #include <array>
    #include <string>

class Player {
    public:
        Player(const std::string &team);
        void setId(int id);
        void setPosition(int x, int y, int dir);
        void setLevel(int lvl);
        void setInventory(const std::array<int, 7> &inventory);
        int getId() const;
        int getX() const;
        int getY() const;
        int getDirection() const;
        int getLevel() const;
        const std::string& getTeam() const;
        const std::array<int, 7> &getInventory() const;
    private:
        int _id;
        int _x;
        int _y;
        int _direction;
        int _lvl;
        std::string _team;
        std::array<int, 7> _inventory{};
};


#endif /* !PLAYER_HPP */