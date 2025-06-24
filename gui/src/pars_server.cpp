/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** pars_messages.cpp
*/

#include "../include/gui.hpp"
#include <sstream>
#include <unordered_map>
#include <functional>

void gui::parse_msz(const std::string &message) {
    std::istringstream iss(message);
    std::string msz;
    iss >> msz;
    int width, height;
    iss >> width >> height;
    map.resize(width, height);
}

void gui::parse_bct(const std::string &message) {
    std::istringstream iss(message);
    std::string bct;
    int x, y;
    std::array<int, 7> resources;
    iss >> bct;
    iss >> x >> y;
    for (int &res : resources)
        iss >> res;
    map.at(x, y).setResources(resources);
}

void gui::parse_server_data(const std::string &message) {
    std::cout << message << std::endl;
    std::string type = message.substr(0, 3);

    static const std::unordered_map<std::string, std::function<void(gui*, const std::string&)>> cmd = {
        {"msz", &gui::parse_msz},
        {"bct", &gui::parse_bct},
    };

    auto it = cmd.find(type);
    if (it != cmd.end()) {
        it->second(this, message);
    } else if (type == "WEL") {
    } else {
        std::cout << "Unknown message type: " << type << std::endl;
    }
}