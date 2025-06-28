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
#include <stdbool.h>
#include <unistd.h>

player_t *create_player(int id, int fd, const char *team, map_t *map)
{
    player_t *player = malloc(sizeof(player_t));

    if (!player) {
        return NULL;
    }
    player->id = id;
    player->fd = fd;
    player->lvl = 1;
    player->dir = rand() % 4;
    player->x = rand() % map->width;
    player->y = rand() % map->height;
    memset(player->inventory, 0, sizeof(player->inventory));
    player->inventory[FOOD] = 10;
    player->action_queue = NULL;
    player->life_remain = 1260;
    player->food_tick = 126;
    player->team = strdup(team);
    printf("Player: id=%d, fd=%d, team=%s, pos=(%d,%d), lvl=%d\n", player->id,
        player->fd, player->team, player->x, player->y, player->lvl);
    return player;
}

void update_player_actions(server_t *server)
{
    player_t *player = NULL;
    action_t *action = NULL;

    for (int i = 0; i < server->player_nb; i++) {
        player = server->players[i];
        if (!player)
            continue;
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

static void send_player_death_to_gui(server_t *server, int player_id)
{
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
        if (server->clients[i].type == CLIENT_GUI &&
            server->pfds[i].fd != FD_NULL) {
            dprintf(server->pfds[i].fd, "pdi %d\n", player_id);
        }
    }
}

static bool consume_food(player_t *player)
{
    if (!player) {
        return false;
    }
    printf("ID: %d, FOOD_TICK: %d, FOOD: %d, LIFE: %d\n",
        player->id, player->food_tick, player->inventory[FOOD],
        player->life_remain);
    player->food_tick--;
    if (player->food_tick <= 0) {
        if (player->inventory[FOOD] > 0) {
            player->inventory[FOOD]--;
            player->life_remain = 1260;
            player->food_tick = 126;
        } else {
            player->life_remain = 0;
        }
    }
    return player->life_remain > 0;
}

void update_player_life(server_t *server)
{
    player_t *player = NULL;

    for (int i = 0; i < server->player_nb; i++) {
        player = server->players[i];
        if (!player) {
            continue;
        }
        player->life_remain--;
        if (!consume_food(player) || player->life_remain <= 0) {
            printf("Player %d is dead\n", player->id);
            send_player_death_to_gui(server, player->id);
            dprintf(player->fd, "dead\n");
            close(player->fd);
            server->pfds[player->fd].fd = FD_NULL;
            free(player->team);
            free(player);
            server->players[i] = NULL;
        }
    }
}
