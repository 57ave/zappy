/*
** EPITECH PROJECT, 2025
** incantation system
** File description:
** Complete incantation system with resource verification
*/

#include "commands.h"
#include "server.h"

elevation_requirements_t get_elevation_requirements(int level)
{
    elevation_requirements_t req = {0, 0, 0, 0, 0, 0, 0};

    if (level == 1)
        req = (elevation_requirements_t){1, 1, 0, 0, 0, 0, 0};
    if (level == 2)
        req = (elevation_requirements_t){2, 1, 1, 1, 0, 0, 0};
    if (level == 3)
        req = (elevation_requirements_t){2, 2, 0, 1, 0, 2, 0};
    if (level == 4)
        req = (elevation_requirements_t){4, 1, 1, 2, 0, 1, 0};
    if (level == 5)
        req = (elevation_requirements_t){4, 1, 2, 1, 3, 0, 0};
    if (level == 6)
        req = (elevation_requirements_t){6, 1, 2, 3, 0, 1, 0};
    if (level == 7)
        req = (elevation_requirements_t){6, 2, 2, 2, 2, 2, 1};
    return req;
}

int count_same_level_players(server_t *server, player_t *player)
{
    int count = 0;

    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i]->x == player->x && 
            server->players[i]->y == player->y &&
            server->players[i]->lvl == player->lvl) {
            count++;
        }
    }
    return count;
}

bool check_tile_resources(server_t *server, player_t *player,
    elevation_requirements_t req)
{
    tile_t *tile = &server->map->tiles[player->y][player->x];

    if (tile->resources[LINEMATE] < req.linemate)
        return false;
    if (tile->resources[DERAUMERE] < req.deraumere)
        return false;
    if (tile->resources[SIBUR] < req.sibur)
        return false;
    if (tile->resources[MENDIANE] < req.mendiane)
        return false;
    if (tile->resources[PHIRAS] < req.phiras)
        return false;
    if (tile->resources[THYSTAME] < req.thystame)
        return false;
    return true;
}
