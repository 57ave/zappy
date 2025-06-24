/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

command_t commands[] = {
    {"Forward", CMD_FORWARD_TIME, cmd_forward},
    {"Right", CMD_RIGHT_TIME, cmd_right},
    {"Left", CMD_LEFT_TIME, cmd_left},
    {"Look", CMD_LOOK_TIME, cmd_look},
    {"Inventory", CMD_INVENTORY_TIME, cmd_inventory},
    {"Broadcast", CMD_BROADCAST_TIME, cmd_broadcast},
    {"Connect_nbr", 0, cmd_connect_nbr},
    {"Fork", CMD_FORK_TIME, cmd_fork},
    {"Eject", CMD_EJECT_TIME, cmd_eject},
    {"Take", CMD_TAKE_TIME, cmd_take},
    {"Set", CMD_SET_TIME, cmd_set},
    {"Incantation", CMD_INCANTATION_TIME, cmd_incantation},
    {NULL, 0, NULL}
};

resource_type_t get_resource_type(const char *name)
{
    if (strcmp(name, "food") == 0) return FOOD;
    if (strcmp(name, "linemate") == 0) return LINEMATE;
    if (strcmp(name, "deraumere") == 0) return DERAUMERE;
    if (strcmp(name, "sibur") == 0) return SIBUR;
    if (strcmp(name, "mendiane") == 0) return MENDIANE;
    if (strcmp(name, "phiras") == 0) return PHIRAS;
    if (strcmp(name, "thystame") == 0) return THYSTAME;
    return -1;
}

const char *get_resource_name(resource_type_t type)
{
    switch (type) {
        case FOOD: return "food";
        case LINEMATE: return "linemate";
        case DERAUMERE: return "deraumere";
        case SIBUR: return "sibur";
        case MENDIANE: return "mendiane";
        case PHIRAS: return "phiras";
        case THYSTAME: return "thystame";
        default: return "unknown";
    }
}

void cmd_forward(server_t *server, player_t *player, char *args)
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

void cmd_right(server_t *server, player_t *player, char *args)
{
    player->direction = (player->direction + 1) % 4;
    dprintf(player->fd, "ok\n");
}

void cmd_left(server_t *server, player_t *player, char *args)
{
    player->direction = (player->direction - 1 + 4) % 4;
    dprintf(player->fd, "ok\n");
}
