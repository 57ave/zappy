/*
** EPITECH PROJECT, 2025
** server
** File description:
** TCP server
*/

#include "server.h"
#include "commands.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/time.h>

int wait_activity(server_t *server, int timeout_ms)
{
    if (timeout_ms == 0)
        timeout_ms = 1;
    return poll(server->pfds, NB_CONNECTION + 1, timeout_ms);
}

void handle_game_tick(server_t *server, server_config_t *config,
    struct timeval *last_tick, int *tick_count)
{
    if (!handle_tick(last_tick, config))
        return;
    update_player_life(server);
    (*tick_count)++;
    if (*tick_count >= 20) {
        generate_resources(server->map);
        send_gui_resource_changes(server);
        *tick_count = 0;
    }
}

static void handle_player_death(player_t *player)
{
    printf("Player %d died\n", player->id);
    dprintf(player->fd, "dead\n");
    close(player->fd);
}

static void consume_food(player_t *player)
{
    player->inventory[FOOD]--;
    player->life_remain = 1260;
    printf("Player %d consumed food, life reset\n", player->id);
}

void update_single_player_life(player_t *player)
{
    if (!player)
        return;
    player->life_remain--;
    if (player->life_remain > 0)
        return;
    if (player->inventory[FOOD] > 0)
        consume_food(player);
    else
        handle_player_death(player);
}
