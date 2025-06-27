/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

void cmd_incantation(server_t *server, player_t *player)
{
    player->lvl++;
    dprintf(player->fd, "Elevation underway\nCurrent level: %d\n", player->lvl);
}


