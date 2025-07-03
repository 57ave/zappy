/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** INetworkParser
*/

#ifndef INETWORKPARSER_HPP_
    #define INETWORKPARSER_HPP_
    #include <string>

class INetworkParser {
    public:
        virtual ~INetworkParser() = default;
        virtual void parse(const std::string &msg, GameState &gameState) = 0;
};

#endif /* !INETWORKPARSER_HPP_ */
