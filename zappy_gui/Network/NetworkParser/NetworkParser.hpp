/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** NetworkParser
*/

#ifndef NETWORKPARSER_HPP_
    #define NETWORKPARSER_HPP_
    #include "../INetworkParser.hpp"
    #include "../../Render/Game/Egg.hpp"
    #include "../../Render/Game/Map.hpp"
    #include "../../Render/Game/Player.hpp"
    #include "../../Render/Game/Tile.hpp"
    #include "../../Render/Game/GameState.hpp"
    #include <string>
    #include <unordered_map>
    #include <functional>
    #include <iostream>
    #include <deque>

class NetworkParser : public INetworkParser {
    public:
        NetworkParser() = default;
        void parse(const std::string &msg, GameState &gameState) override;
        void parse_msz(const std::string &msg, GameState &gameState);
        void parse_bct(const std::string &msg, GameState &gameState);
        void parse_tna(const std::string &msg, GameState &gameState);
        void parse_pnw(const std::string &msg, GameState &gameState);
        void parse_ppo(const std::string &msg, GameState &gameState);
        void parse_plv(const std::string &msg, GameState &gameState);
        void parse_pin(const std::string &msg, GameState &gameState);
        void parse_sgt(const std::string &msg, GameState &gameState);
        void parse_sst(const std::string &msg, GameState &gameState);
        void parse_pgt(const std::string &msg, GameState &gameState);
        void parse_pdr(const std::string &msg, GameState &gameState);
        void parse_pfk(const std::string &msg, GameState &gameState);
        void parse_enw(const std::string &msg, GameState &gameState);
        void parse_ebo(const std::string &msg, GameState &gameState);
        void parse_edi(const std::string &msg, GameState &gameState);
        void parse_pbc(const std::string &msg, GameState &gameState);
        void parse_pdi(const std::string &msg, GameState &gameState);
        void parse_pex(const std::string &msg, GameState &gameState);
        void parse_seg(const std::string &msg, GameState &gameState);
        void parse_smg(const std::string &msg, GameState &gameState);
        void parse_suc(const std::string &msg, GameState &gameState);
        void parse_sbp(const std::string &msg, GameState &gameState);
        void parse_pic(const std::string &msg, GameState &gameState);
        void parse_pie(const std::string &msg, GameState &gameState);
    private :
        void addPopMessage(const std::string& msg, GameState &gameState);
};

#endif /* !NETWORKPARSER_HPP_ */
