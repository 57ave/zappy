/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** pars_messages.cpp
*/

#include "../include/gui.hpp"

void gui::parse_server_data(const std::string &message) {
    std::string type = message.substr(0, 3);
    if (type == "msz") {
        size_t space_pos = message.find(' ');
        if (space_pos != std::string::npos) {
            std::string width_str = message.substr(4, space_pos - 4);
            std::string height_str = message.substr(space_pos + 1);
            map.resize(std::stoi(width_str), std::stoi(height_str));
        }
    }
}
