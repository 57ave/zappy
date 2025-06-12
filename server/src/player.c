/*
** EPITECH PROJECT, 2025
** player.c
** File description:
** player
*/
#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

player_t *create_player(int id, int fd, const char *team, map_t *map)
{
    player_t *player = malloc(sizeof(player_t));

    if (!player) {
        return NULL;
    }
    player->id = id;
    player->fd = fd;
    player->lvl = 1;
    player->direction = rand() % 4;
    player->x = rand() % map->width;
    player->y = rand() % map->height;
    memset(player->inventory, 0, sizeof(player->inventory));
    player->inventory[FOOD] = 10;
    player->action_queue = NULL;
    player->life_remain = 1260;
    player->team = strdup(team);
    printf("Player: id=%d, fd=%d, team=%s, pos=(%d,%d), lvl=%d\n", player->id,
        player->fd, player->team, player->x, player->y, player->lvl);
    return player;
}
