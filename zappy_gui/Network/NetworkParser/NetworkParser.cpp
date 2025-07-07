/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** NetworkParser
*/

#include "NetworkParser.hpp"
#include <sstream>
#include <algorithm>
#include <array>

static const std::string resourceNames[] = {
    "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
};

void NetworkParser::addPopMessage(const std::string& msg, GameState &gameState)
{
    const size_t maxMessages = 10;
    if (gameState._popMessages.size() >= maxMessages) {
        gameState._popMessages.pop_front();
    }
    gameState._popMessages.push_back({msg});
}

void NetworkParser::parse_msz(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string msz;
    iss >> msz;
    int width, height;
    iss >> width >> height;
    gameState.map.resize(width, height);
}

void NetworkParser::parse_bct(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string bct;
    int x, y;
    std::array<int, 7> resources;
    iss >> bct;
    iss >> x >> y;
    for (int &res : resources)
        iss >> res;
    if (gameState.map.getWidth() == 0 || gameState.map.getHeight() == 0)
        return; 
    gameState.map.at(x, y).setResources(resources);
}

void NetworkParser::parse_tna(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string tna;
    std::string team_name;
    iss >> tna >> team_name;
    if (!team_name.empty())
        gameState.teams.push_back(team_name);
}

void NetworkParser::parse_pnw(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pnw;
    int id, x, y, direction, level;
    std::string team_name;
    iss >> pnw >> id >> x >> y >> direction >> level >> team_name;
    if (team_name.empty()) {
        return;
    }
    gameState.players.push_back(Player(id, team_name));
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            player.setPosition(x, y, direction);
            player.setLevel(level);
            return;
        }
    }
}

void NetworkParser::parse_ppo(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string ppo;
    int id, x, y, direction;
    iss >> ppo >> id >> x >> y >> direction;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            player.setPosition(x, y, direction);
            return;
        }
    }
}

void NetworkParser::parse_plv(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string plv;
    int id, level;
    iss >> plv >> id >> level;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            player.setLevel(level);
            return;
        }
    }

}

void NetworkParser::parse_pin(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pin;
    int id, x, y;
    std::array<int, 7> resources;
    iss >> pin >> id >> x >> y;
    for (int &res : resources)
        iss >> res;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            player.setInventory(resources);
            break;
        }
    }
}

void NetworkParser::parse_sgt(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string sgt;
    int time;
    iss >> sgt >> time;
    gameState.timeUnit = time;
}

void NetworkParser::parse_sst(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string sgt;
    int time;
    iss >> sgt >> time;
    gameState.timeUnit = time;
}

void NetworkParser::parse_pgt(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pgt;
    int id, resourceType;
    iss >> pgt >> id >> resourceType;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            addPopMessage("Player " + std::to_string(id) + " is taking " + resourceNames[resourceType], gameState);
        }
    }
}

void NetworkParser::parse_pdr(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pdr;
    int id, resourceType;
    iss >> pdr >> id >> resourceType;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            addPopMessage("Player " + std::to_string(id) + " is dropping " + resourceNames[resourceType], gameState);
        }
    }
}

void NetworkParser::parse_pfk(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pfk;
    int id;
    iss >> pfk >> id;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            addPopMessage("Egg laying by the player " + std::to_string(id), gameState);
            gameState.eggs.push_back(Egg(gameState.eggs.back().getId() + 1, id, player.getX(), player.getY()));
        }
    }
}

void NetworkParser::parse_enw(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string enw;
    int eggId, playerId, x, y;
    iss >> enw >> eggId >> playerId >> x >> y;
    gameState.eggs.push_back(Egg(eggId, playerId, x, y));
    
    for (auto &player : gameState.players) {
        if (player.getId() == gameState.eggs.back().getPlayerId()) {
            addPopMessage("Egg laying by the player " + std::to_string(gameState.eggs.back().getPlayerId()), gameState);
            gameState.eggs.push_back(Egg(eggId, playerId, x, y));
        }
    }
    parse_pfk("pfk " + std::to_string(playerId) + "\n", gameState);
}

void NetworkParser::parse_ebo(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string ebo;
    int eggId;
    iss >> ebo >> eggId;
    for (auto &egg : gameState.eggs) {
        if (egg.getId() == eggId) {
            addPopMessage("Egg " + std::to_string(eggId) + " is connected to player " + std::to_string(egg.getPlayerId()), gameState);
            return;
        }
    }
}

void NetworkParser::parse_edi(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string edi;
    int eggId;
    iss >> edi >> eggId;
    gameState.eggs.erase(std::remove_if(gameState.eggs.begin(), gameState.eggs.end(),
        [eggId](const Egg &egg) { return egg.getId() == eggId; }), gameState.eggs.end());
    addPopMessage("Egg " + std::to_string(eggId) + " is dead", gameState);
}

void NetworkParser::parse_pbc(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pbc;
    int id;
    std::string broadcastMessage;
    iss >> pbc >> id;
    std::getline(iss, broadcastMessage);
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            addPopMessage("Player " + std::to_string(id) + " is broadcast:\n" + broadcastMessage, gameState);
            return;
        }
    }
}

void NetworkParser::parse_pdi(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pdi;
    int id;
    iss >> pdi >> id;
    gameState.players.erase(std::remove_if(gameState.players.begin(), gameState.players.end(),
        [id](const Player &player) { return player.getId() == id; }), gameState.players.end());
    addPopMessage("Player " + std::to_string(id) + " is dead", gameState);
}

void NetworkParser::parse_pex(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pex;
    int id;
    iss >> pex >> id;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            addPopMessage("Player " + std::to_string(id) + " has been ejected", gameState);
            return;
        }
    }
}

void NetworkParser::parse_seg(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string seg;
    int id;
    iss >> seg >> id;
    for (auto &player : gameState.players) {
        if (player.getId() == id) {
            gameState.winnerTeam = player.getTeam();
            gameState.endGame = true;
            return;
        }
    }
}

void NetworkParser::parse_smg(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string smg;
    std::string msg;
    iss >> smg;
    std::getline(iss, msg);
    addPopMessage("Server: " + msg, gameState);
}

void NetworkParser::parse_suc(const std::string &message, GameState &gameState)
{
    addPopMessage("Unknown command", gameState);
}

void NetworkParser::parse_sbp(const std::string &message, GameState &gameState)
{
    addPopMessage("Command parameter", gameState);
}

void NetworkParser::parse_pic(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pic;
    int x, y, playerLvl, idPlayer1;
    iss >> pic >> x >> y >> playerLvl >> idPlayer1;
    std::vector<int> playerIds;
    int id;
    while (iss >> id) {
        playerIds.push_back(id);
    }
    addPopMessage("Player " + std::to_string(idPlayer1) + " start an incantation", gameState);
}

void NetworkParser::parse_pie(const std::string &message, GameState &gameState)
{
    std::istringstream iss(message);
    std::string pie;
    int x, y;
    std::string result;
    iss >> pie >> x >> y >> result;
    if (result == "ko") {
        addPopMessage("Incantation failed", gameState);
    } else {
        addPopMessage("Incantation succeeded", gameState);
    }
}

void NetworkParser::parse(const std::string &msg, GameState &gameState)
{
    std::istringstream iss(msg);
    std::string command;
    iss >> command;
    using ParseFunc = void(NetworkParser::*)(const std::string&, GameState &);
    static const std::unordered_map<std::string, ParseFunc> cmd = {
        {"msz", &NetworkParser::parse_msz},
        {"bct", &NetworkParser::parse_bct},
        {"tna", &NetworkParser::parse_tna},
        {"pnw", &NetworkParser::parse_pnw},
        {"ppo", &NetworkParser::parse_ppo},
        {"plv", &NetworkParser::parse_plv},
        {"pin", &NetworkParser::parse_pin},
        {"sgt", &NetworkParser::parse_sgt},
        {"sst", &NetworkParser::parse_sst},
        {"pgt", &NetworkParser::parse_pgt},
        {"pdr", &NetworkParser::parse_pdr},
        {"pfk", &NetworkParser::parse_pfk},
        {"enw", &NetworkParser::parse_enw},
        {"ebo", &NetworkParser::parse_ebo},
        {"edi", &NetworkParser::parse_edi},
        {"pbc", &NetworkParser::parse_pbc},
        {"pdi", &NetworkParser::parse_pdi},
        {"pex", &NetworkParser::parse_pex},
        {"seg", &NetworkParser::parse_seg},
        {"smg", &NetworkParser::parse_smg},
        {"suc", &NetworkParser::parse_suc},
        {"sbp", &NetworkParser::parse_sbp},
        {"pic", &NetworkParser::parse_pic},
        {"pie", &NetworkParser::parse_pie}
    };

    auto it = cmd.find(command);
    if (it != cmd.end()) {
        (this->*(it->second))(msg, gameState);
    } else {
        addPopMessage("Unhandled command: " + command, gameState);
    }
}
