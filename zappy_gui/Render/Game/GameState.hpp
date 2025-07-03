/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GameState
*/

#ifndef GAMESTATE_HPP_
    #define GAMESTATE_HPP_
    #include <vector>
    #include <mutex>
    #include "Egg.hpp"
    #include "Map.hpp"
    #include "Player.hpp"
    #include <queue>

struct GameState {
    Map map;
    std::vector<Player> players;
    std::vector<Egg> eggs;
    std::vector<std::string> teams;
    int timeUnit = 0;
    bool endGame = false;
    std::string winnerTeam;
    std::mutex mutex;
    std::deque<std::string> _popMessages;
};

#endif /* !GAMESTATE_HPP_ */
