/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

void consume_incantation_resources(server_t *server, player_t *player,
    elevation_requirements_t req)
{
    tile_t *tile = &server->map->tiles[player->y][player->x];

    tile->resources[LINEMATE] -= req.linemate;
    tile->resources[DERAUMERE] -= req.deraumere;
    tile->resources[SIBUR] -= req.sibur;
    tile->resources[MENDIANE] -= req.mendiane;
    tile->resources[PHIRAS] -= req.phiras;
    tile->resources[THYSTAME] -= req.thystame;
}

void elevate_all_participants(server_t *server, player_t *initiator)
{
    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i]->x == initiator->x &&
            server->players[i]->y == initiator->y &&
            server->players[i]->lvl == initiator->lvl) {
            server->players[i]->lvl++;
            dprintf(server->players[i]->fd, "Current level: %d\n",
                server->players[i]->lvl);
        }
    }
}

bool validate_incantation_requirements(server_t *server, player_t *player,
    elevation_requirements_t req)
{
    int player_count = count_same_level_players(server, player);

    if (player_count < req.required_players) {
        printf("Incantation failed: need %d players, got %d\n",
            req.required_players, player_count);
        return false;
    }
    if (!check_tile_resources(server, player, req)) {
        printf("Incantation failed: insufficient resources\n");
        return false;
    }
    return true;
}

void cmd_incantation(server_t *s, player_t *p)
{
    elevation_requirements_t req;

    if (p->lvl < 1 || p->lvl > 7) {
        dprintf(p->fd, "ko\n");
        return;
    }
    req = get_elevation_requirements(p->lvl);
    dprintf(p->fd, "Elevation underway\n");
    if (!validate_incantation_requirements(s, p, req)) {
        dprintf(p->fd, "ko\n");
        dprintf(s->gui_fd, "pie %d %d %d\n", p->x, p->y, 0);
        return;
    }
    consume_incantation_resources(s, p, req);
    elevate_all_participants(s, p);
    if (s->gui_fd > 0) {
        dprintf(s->gui_fd, "pic %d %d %d %d\n", p->x, p->y, p->lvl, p->id);
    }
    printf("Incantation: %d player level %d\n", req.required_players, p->lvl);
    dprintf(s->gui_fd, "pie %d %d %d\n", p->x, p->y, 1);
}
