/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** pars_messages.cpp
*/

#include "../include/gui.hpp"
#include <sstream>

void gui::parse_server_data(const std::string &message) {
    std::cout << message << std::endl;
    std::string type = message.substr(0, 3);
    if (type == "WEL") {
    } else if (type == "msz") {
        size_t space_pos = message.find(' ');
        if (space_pos != std::string::npos) {
            std::string width_str = message.substr(4, space_pos - 4);
            std::string height_str = message.substr(space_pos + 1);
            map.resize(std::stoi(width_str), std::stoi(height_str));
        }
    } else if (type == "bct") {
        std::istringstream iss(message);
        std::string bct;
        int x, y;
        std::array<int, 7> resources;
        iss >> bct;
        iss >> x >> y;
        for (int &res : resources)
            iss >> res;
        map.at(x, y).setResources(resources);
    } else 
        std::cout << "Unknown message type: " << type << std::endl;
}
