/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GuiCore
*/

#include "GuiCore.hpp"
#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>

GuiCore::GuiCore(std::unique_ptr<INetwork> network, std::unique_ptr<IRender> render) : _network(std::move(network)), _render(std::move(render)), _running(false)
{
}

void GuiCore::run()
{
    _running = true;
    _network->startReceive();

    while (_running) {
        processNetworkMsg();
        updateGameState();
        _render->render();
    }
    _network->stopReceive();
}

void GuiCore::processNetworkMsg()
{
    std::string msg;

    while (_network->tryPopMessage(msg)) {
        _parser.parse(msg, _gameState);
    }
}

void GuiCore::updateGameState()
{
    if (_gameState.endGame == true) {
        _running = false;
    }
}

void GuiCore::startNetworkReceive()
{
    _network->startReceive();
}

bool GuiCore::tryPopMessage(std::string& msg)
{
    return _network->tryPopMessage(msg);
}

bool GuiCore::parseArgs(int argc, char** argv, int& port, std::string& hostname)
{
    if (argc != 5) {
        std::cerr << "Usage: ./zappy_gui -p <port> -h <hostname>" << std::endl;
        return false;
    }
    if (std::strcmp(argv[1], "-p") != 0 || std::strcmp(argv[3], "-h") != 0) {
        std::cerr << "Invalid argument use: -p <port> -h <host>" << std::endl;
        return false;
    }
    port = std::atoi(argv[2]);
    if (port <= 0) {
        std::cerr << "Invalid port number" << std::endl;
        return false;
    }
    hostname = argv[4];
    if (hostname.empty()) {
        std::cerr << "Hostname cannot be empty" << std::endl;
        return false;
    }
    return true;
}

