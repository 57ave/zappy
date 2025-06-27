/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

resource_type_t get_resource_type(const char *name)
{
    if (strcmp(name, "food") == 0)
        return FOOD;
    if (strcmp(name, "linemate") == 0)
        return LINEMATE;
    if (strcmp(name, "deraumere") == 0)
        return DERAUMERE;
    if (strcmp(name, "sibur") == 0)
        return SIBUR;
    if (strcmp(name, "mendiane") == 0)
        return MENDIANE;
    if (strcmp(name, "phiras") == 0)
        return PHIRAS;
    if (strcmp(name, "thystame") == 0)
        return THYSTAME;
    return -1;
}

const char *get_resource_name(resource_type_t type)
{
    switch (type) {
        case FOOD:
            return "food";
        case LINEMATE:
            return "linemate";
        case DERAUMERE:
            return "deraumere";
        case SIBUR:
            return "sibur";
        case MENDIANE:
            return "mendiane";
        case PHIRAS:
            return "phiras";
        case THYSTAME:
            return "thystame";
        default:
            return "unknown";
    }
}

void cmd_forward(server_t *server, player_t *player)
{
    int new_x = player->x;
    int new_y = player->y;

    switch (player->direction) {
        case UP:
            new_y = (player->y - 1 + server->map->height) % server->map->height;
            break;
        case RIGHT:
            new_x = (player->x + 1) % server->map->width;
            break;
        case DOWN:
            new_y = (player->y + 1) % server->map->height;
            break;
        case LEFT:
            new_x = (player->x - 1 + server->map->width) % server->map->width;
            break;
    }
    player->x = new_x;
    player->y = new_y;
    dprintf(player->fd, "ok\n");
}

void cmd_right(player_t *player)
{
    player->direction = (player->direction + 1) % 4;
    dprintf(player->fd, "ok\n");
}

void cmd_left(player_t *player)
{
    player->direction = (player->direction - 1 + 4) % 4;
    dprintf(player->fd, "ok\n");
}
