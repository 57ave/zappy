/*
** EPITECH PROJECT, 2025
** player.c
** File description:
** player
*/
#include "player.h"
#include "server.h"
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

void add_action_to_queue(player_t *player, const char *cmd, int time)
{
    action_t *new = malloc(sizeof(action_t));
    action_t *tmp = NULL;

    new->command = strdup(cmd);
    new->remaining_ticks = time;
    new->next = NULL;
    if (!player->action_queue) {
        player->action_queue = new;
        return;
    }
    tmp = player->action_queue;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
}

void update_player_actions(server_t *server)
{
    player_t *player = NULL;
    action_t *action = NULL;

    for (int i = 0; i < server->player_nb; i++) {
        player = server->players[i];
        action = player->action_queue;
        if (!action) {
            continue;
        }
        action->remaining_ticks--;
        if (action->remaining_ticks <= 0) {
            printf("Action prête : %s player %d\n",
                action->command, player->id);
        }
    }
}
