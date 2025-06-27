/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

static void execute_action_commands(server_t *server, player_t *player,
    const char *cmd_name, const char *safe_args)
{
    if (strcmp(cmd_name, "Fork") == 0)
        cmd_fork(server, player);
    if (strcmp(cmd_name, "Eject") == 0)
        cmd_eject(server, player);
    if (strcmp(cmd_name, "Take") == 0)
        cmd_take(server, player, (char *)safe_args);
    if (strcmp(cmd_name, "Set") == 0)
        cmd_set(server, player, (char *)safe_args);
    if (strcmp(cmd_name, "Incantation") == 0)
        cmd_incantation(server, player);
}

static void execute_single_command(server_t *server, player_t *player,
    const char *cmd_name, const char *args)
{
    const char *safe_args = args ? args : "";

    execute_movement_and_info_commands(server, player, cmd_name, safe_args);
    execute_action_commands(server, player, cmd_name, safe_args);
}

void remove_action_from_queue(player_t *player)
{
    action_t *action = player->action_queue;

    player->action_queue = action->next;
    free(action->command);
    free(action);
}

void process_player_action(server_t *server, player_t *player)
{
    action_t *action = player->action_queue;
    char *command_copy = NULL;
    char *cmd_name = NULL;
    char *args = NULL;

    if (!action || action->remaining_ticks > 0)
        return;
    command_copy = create_command_copy(action->command);
    if (!command_copy)
        return;
    parse_command_args(command_copy, &cmd_name, &args);
    if (cmd_name) {
        printf("Executing completed action: %s for player %d\n",
            cmd_name, player->id);
        execute_single_command(server, player, cmd_name, args);
    }
    free(command_copy);
    remove_action_from_queue(player);
}

void process_completed_actions(server_t *server)
{
    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i])
            process_player_action(server, server->players[i]);
    }
}
