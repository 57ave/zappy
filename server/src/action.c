/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

void execute_command(server_t *server, player_t *player, char *command)
{
    char *cmd_name;
    char *args;
    command_t *found_cmd;
    
    parse_command(command, &cmd_name, &args);
    
    if (!cmd_name) {
        send_command_error(player);
        return;
    }
    
    found_cmd = find_command(cmd_name);
    if (found_cmd) {
        handle_found_command(server, player, found_cmd, command, args);
    } else {
        send_command_error(player);
    }
}

void execute_action_command(server_t *server, player_t *player, char *command)
{
    char *cmd_name = strtok(command, " ");
    char *args = strtok(NULL, "");
    
    for (int j = 0; commands[j].name != NULL; j++) {
        if (strcmp(cmd_name, commands[j].name) == 0) {
            commands[j].execute(server, player, args ? args : "");
            break;
        }
    }
}

void remove_action_from_queue(player_t *player, action_t *action)
{
    player->action_queue = action->next;
    free(action->command);
    free(action);
}

void process_player_action(server_t *server, player_t *player)
{
    action_t *action = player->action_queue;
    
    if (action && action->remaining_ticks <= 0) {
        execute_action_command(server, player, action->command);
        remove_action_from_queue(player, action);
    }
}

void process_completed_actions(server_t *server)
{
    for (int i = 0; i < server->player_nb; i++) {
        process_player_action(server, server->players[i]);
    }
}
