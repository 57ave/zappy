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

void cmd_incantation(server_t *server, player_t *player, char *args)
{
    player->lvl++;
    dprintf(player->fd, "Elevation underway\nCurrent level: %d\n", player->lvl);
}

void parse_command(char *command, char **cmd_name, char **args)
{
    *cmd_name = strtok(command, " \n");
    *args = strtok(NULL, "\n");
}

void send_command_error(player_t *player)
{
    dprintf(player->fd, "ko\n");
}

command_t *find_command(char *cmd_name)
{
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(cmd_name, commands[i].name) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}

void handle_found_command(server_t *server, player_t *player, 
                                command_t *cmd, char *original_command, char *args)
{
    if (cmd->time > 0) {
        add_action_to_queue(player, original_command, cmd->time);
    } else {
        cmd->execute(server, player, args ? args : "");
    }
}
