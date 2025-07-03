/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Main
*/
#include <iostream>
#include "Core/GuiCore.hpp"
#include "Network/NetworkClient/NetworkClient.hpp"
#include "Render/RenderGui.hpp"

int main(int argc, char** argv)
{
    int port;
    std::string hostname;
    std::string msg;

    if (!GuiCore::parseArgs(argc, argv, port, hostname)) {
        return 84;
    }
    try {
        auto network = std::make_unique<NetworkClient>(hostname, port);
        auto render = std::make_unique<Render>();
        render->init(1920, 1080);

        GuiCore gui(std::move(network), std::move(render));
        gui.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}

