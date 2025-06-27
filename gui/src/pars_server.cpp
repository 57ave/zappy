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

static std::string resourceNames[] = {
    "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
};

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

void gui::parse_tna(const std::string &message) {
    std::istringstream iss(message);
    std::string tna;
    std::string team_name;
    iss >> tna >> team_name;
    if (!team_name.empty())
        teams.push_back(team_name);
}

void gui::parse_pnw(const std::string &message) {
    std::istringstream iss(message);
    std::string pnw;
    int id, x, y, direction, level;
    std::string team_name;
    iss >> pnw >> id >> x >> y >> direction >> level >> team_name;
    if (team_name.empty()) {
        return;
    }
    players.push_back(Player(id, team_name));
    for (auto &player : players) {
        if (player.getId() == id) {
            player.setPosition(x, y, direction);
            player.setLevel(level);
            return;
        }
    }
}

void gui::parse_ppo(const std::string &message) {
    std::istringstream iss(message);
    std::string ppo;
    int id, x, y, direction;
    iss >> ppo >> id >> x >> y >> direction;
    for (auto &player : players) {
        if (player.getId() == id) {
            player.setPosition(x, y, direction);
            return;
        }
    }
}

void gui::parse_plv(const std::string &message) {
    std::istringstream iss(message);
    std::string plv;
    int id, level;
    iss >> plv >> id >> level;
    for (auto &player : players) {
        if (player.getId() == id) {
            player.setLevel(level);
            return;
        }
    }

}

void gui::parse_pin(const std::string &message) {
    std::istringstream iss(message);
    std::string pin;
    int id, x, y;
    std::array<int, 7> resources;
    iss >> pin >> id >> x >> y;
    for (int &res : resources)
        iss >> res;
    for (auto &player : players) {
        if (player.getId() == id) {
            player.setInventory(resources);
            break;
        }
    }
}

void gui::parse_sgt(const std::string &message) {
    std::istringstream iss(message);
    std::string sgt;
    int time;
    iss >> sgt >> time;
    timeGame = time;
}

void gui::parse_sst(const std::string &message) {
    std::istringstream iss(message);
    std::string sgt;
    int time;
    iss >> sgt >> time;
    timeGame = time;
}

void gui::parse_pgt(const std::string &message) {
    std::istringstream iss(message);
    std::string pgt;
    int id, resourceType;
    iss >> pgt >> id >> resourceType;
    for (auto &player : players) {
        if (player.getId() == id) {
            std::string cmd = "pin " + std::to_string(id) + "\n";
            player.animation = true;
            player.animationMessage = "Player " + std::to_string(id) + " is taking " + resourceNames[resourceType];
            player.animationClock.restart();
        }
    }
}

void gui::parse_pdr(const std::string &message) {
    std::istringstream iss(message);
    std::string pdr;
    int id, resourceType;
    iss >> pdr >> id >> resourceType;
    for (auto &player : players) {
        if (player.getId() == id) {
            std::string cmd = "pin " + std::to_string(id) + "\n";
            player.animation = true;
            player.animationMessage = "Player " + std::to_string(id) + " is dropping " + resourceNames[resourceType];
            player.animationClock.restart();
        }
    }
}

void gui::parse_server_data(const std::string &message) {
    std::string type = message.substr(0, 3);

    static const std::unordered_map<std::string, std::function<void(gui*, const std::string&)>> cmd = {
        {"msz", &gui::parse_msz},
        {"bct", &gui::parse_bct},
        {"tna", &gui::parse_tna},
        {"pnw", &gui::parse_pnw},
        {"ppo", &gui::parse_ppo},
        {"plv", &gui::parse_plv},
        {"pin", &gui::parse_pin},
        {"sgt", &gui::parse_sgt},
        {"sst", &gui::parse_sst},
        {"pgt", &gui::parse_pgt},
        {"pdr", &gui::parse_pdr}
    };

    auto it = cmd.find(type);
    if (it != cmd.end()) {
        it->second(this, message);
    } else if (type == "WEL") {
    } else {
        std::cout << "Unknown message type: " << type << std::endl;
    }
}