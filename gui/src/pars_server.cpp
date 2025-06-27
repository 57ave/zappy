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
            addPopMessage("Player " + std::to_string(id) + " is taking " + resourceNames[resourceType]);
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
            addPopMessage("Player " + std::to_string(id) + " is dropping " + resourceNames[resourceType]);
        }
    }
}

void gui::parse_pfk(const std::string &message) {
    std::istringstream iss(message);
    std::string pfk;
    int id;
    iss >> pfk >> id;
    for (auto &player : players) {
        if (player.getId() == id) {
            addPopMessage("Egg laying by the player " + std::to_string(id));
        }
    }
}

void gui::parse_enw(const std::string &message) {
    std::istringstream iss(message);
    std::string enw;
    int eggId, playerId, x, y;
    iss >> enw >> eggId >> playerId >> x >> y;
    eggs.push_back(Egg(eggId, playerId, x, y));
    
    for (auto &player : players) {
        if (player.getId() == eggs.back().getPlayerId()) {
            addPopMessage("Egg laying by the player " + std::to_string(eggs.back().getPlayerId()));
        }
    }
    parse_pfk("pfk " + std::to_string(playerId) + "\n");
}

void gui::parse_ebo(const std::string &message) {
    std::istringstream iss(message);
    std::string ebo;
    int eggId;
    iss >> ebo >> eggId;
    for (auto &egg : eggs) {
        if (egg.getId() == eggId) {
            addPopMessage("Egg " + std::to_string(eggId) + " is connected to player " + std::to_string(egg.getPlayerId()));
            return;
        }
    }
}

void gui::parse_edi(const std::string &message) {
    std::istringstream iss(message);
    std::string edi;
    int eggId;
    iss >> edi >> eggId;
    eggs.erase(std::remove_if(eggs.begin(), eggs.end(),
        [eggId](const Egg &egg) { return egg.getId() == eggId; }), eggs.end());
    addPopMessage("Egg " + std::to_string(eggId) + " is dead");
}

void gui::parse_pbc(const std::string &message) {
    std::istringstream iss(message);
    std::string pbc;
    int id;
    std::string broadcastMessage;
    iss >> pbc >> id;
    std::getline(iss, broadcastMessage);
    for (auto &player : players) {
        if (player.getId() == id) {
            addPopMessage("Player " + std::to_string(id) + " is broadcast:\n" + broadcastMessage);
            return;
        }
    }
}

void gui::parse_pdi(const std::string &message) {
    std::istringstream iss(message);
    std::string pdi;
    int id;
    iss >> pdi >> id;
    players.erase(std::remove_if(players.begin(), players.end(),
        [id](const Player &player) { return player.getId() == id; }), players.end());
    addPopMessage("Player " + std::to_string(id) + " is dead");
}

void gui::parse_pex(const std::string &message) {
    std::istringstream iss(message);
    std::string pex;
    int id;
    iss >> pex >> id;
    for (auto &player : players) {
        if (player.getId() == id) {
            addPopMessage("Player " + std::to_string(id) + " has been ejected");
            return;
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
        {"pdr", &gui::parse_pdr},
        {"pfk", &gui::parse_pfk},
        {"enw", &gui::parse_enw},
        {"ebo", &gui::parse_ebo},
        {"edi", &gui::parse_edi},
        {"pbc", &gui::parse_pbc},
        {"pdi", &gui::parse_pdi},
        {"pex", &gui::parse_pex}
    };

    auto it = cmd.find(type);
    if (it != cmd.end()) {
        it->second(this, message);
    } else if (type == "WEL") {
    } else {
        std::cout << "Unknown message type: " << type << std::endl;
    }
}