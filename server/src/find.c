/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

void cmd_incantation(server_t *server, player_t *p)
{
    p->lvl++;
    dprintf(p->fd, "Elevation underway\nCurrent level: %d\n", p->lvl);
    dprintf(server->gui_fd, "pic %d %d %d %d\n", p->x, p->y, p->lvl, p->id);
}
