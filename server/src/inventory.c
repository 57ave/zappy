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

char *build_inventory_response(player_t *player)
{
    size_t needed_size = calculate_total_size(player);
    char *response = malloc(needed_size);
    bool first = true;
    
    if (!response)
        return NULL;
    
    strcpy(response, "[");
    
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (!process_single_item(player, i, response, &first)) {
            free(response);
            return NULL;
        }
    }
    
    strcat(response, "]");
    return response;
}

void cmd_inventory(server_t *server, player_t *player, char *args)
{
    char *response = build_inventory_response(player);
    
    if (!response) {
        dprintf(player->fd, "ko\n");
        return;
    }
    
    dprintf(player->fd, "%s\n", response);
    free(response);
}

void cmd_broadcast(server_t *server, player_t *player, char *args)
{
    int direction = 0;

    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i] != player) {
            dprintf(server->players[i]->fd, "message %d,%s\n", direction, args);
        }
    }
    dprintf(player->fd, "ok\n");
}

void cmd_connect_nbr(server_t *server, player_t *player, char *args)
{
    team_t *team = find_team(player->team, server->config);

    if (team) {
        int available_slots = team->max_players - team->actual_players;
        dprintf(player->fd, "%d\n", available_slots);
    } else {
        dprintf(player->fd, "0\n");
    }
}

void cmd_fork(server_t *server, player_t *player, char *args)
{
    team_t *team = find_team(player->team, server->config);

    if (team) {
        team->eggs_available++;
        dprintf(player->fd, "ok\n");
    } else {
        dprintf(player->fd, "ko\n");
    }
}