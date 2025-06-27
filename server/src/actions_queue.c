/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "player.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

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