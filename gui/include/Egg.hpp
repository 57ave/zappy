/*
** EPITECH PROJECT, 2025
** Egg
** File description:
** Egg
*/

#ifndef EGG_HPP
    #define EGG_HPP

class Egg {
    public:
        Egg(int id, int playerId, int x, int y);
        int getId() const;
        int getPlayerId() const;
        int getX() const;
        int getY() const;
    private:
        int _id;
        int _playerId;
        int _x;
        int _y;
    };

#endif /* !EGG_HPP */