/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Main
*/
#include <iostream>
#include "Core/GuiCore.hpp"
#include "Network/NetworkClient/NetworkClient.hpp"

int main(int argc, char** argv)
{
    int port;
    std::string hostname;
    std::string msg;

    if (!GuiCore::parseArgs(argc, argv, port, hostname)) {
        return 84;
    }
    try {
        NetworkClient networkClient(hostname, port);
        while (true) {
            networkClient.receiveMessage();
            while (networkClient.tryPopMessage(msg)) {
                std::cout << "Received: " << msg << std::endl;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}

