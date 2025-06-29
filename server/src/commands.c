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

static void send_forward_response(server_t *server, player_t *player)
{
    dprintf(player->fd, "ok\n");
    dprintf(server->gui_fd, "ppo %d %d %d %d\n",
            player->id, player->x, player->y, player->dir + 1);
}

void cmd_forward(server_t *s, player_t *p)
{
    int new_x = p->x;
    int new_y = p->y;

    switch (p->dir) {
        case UP:
            new_y = (p->y - 1 + s->map->height) % s->map->height;
            break;
        case RIGHT:
            new_x = (p->x + 1) % s->map->width;
            break;
        case DOWN:
            new_y = (p->y + 1) % s->map->height;
            break;
        case LEFT:
            new_x = (p->x - 1 + s->map->width) % s->map->width;
            break;
    }
    p->x = new_x;
    p->y = new_y;
    send_forward_response(s, p);
}

void cmd_right(server_t *s, player_t *p)
{
    p->dir = (p->dir + 1) % 4;
    dprintf(p->fd, "ok\n");
    dprintf(s->gui_fd, "ppo %d %d %d %d\n", p->id, p->x, p->y, p->dir + 1);
}

void cmd_left(server_t *s, player_t *p)
{
    p->dir = (p->dir - 1 + 4) % 4;
    dprintf(p->fd, "ok\n");
    dprintf(s->gui_fd, "ppo %d %d %d %d\n", p->id, p->x, p->y, p->dir + 1);
}
