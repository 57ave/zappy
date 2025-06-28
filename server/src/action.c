/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

char *create_command_copy(const char *original)
{
    char *copy = NULL;
    size_t len = strlen(original) + 1;

    copy = malloc(len);
    if (!copy)
        return NULL;
    strncpy(copy, original, len - 1);
    copy[len - 1] = '\0';
    return copy;
}

void parse_command_args(char *command_copy, char **cmd_name, char **args)
{
    *cmd_name = strtok(command_copy, " ");
    *args = strtok(NULL, "");
}

void execute_movement_and_info_commands(server_t *server, player_t *player,
    const char *cmd_name, const char *safe_args)
{
    if (strcmp(cmd_name, "Forward") == 0)
        cmd_forward(server, player);
    if (strcmp(cmd_name, "Right") == 0)
        cmd_right(server, player);
    if (strcmp(cmd_name, "Left") == 0)
        cmd_left(server, player);
    if (strcmp(cmd_name, "Look") == 0)
        cmd_look(server, player);
    if (strcmp(cmd_name, "Inventory") == 0)
        cmd_inventory(player);
    if (strcmp(cmd_name, "Broadcast") == 0)
        cmd_broadcast(server, player, (char *)safe_args);
}

void handle_movement_commands(player_t *player, const char *cmd_name,
    const char *original_command)
{
    if (strcmp(cmd_name, "Forward") == 0)
        add_command_with_time(player, original_command, CMD_FORWARD_TIME);
    if (strcmp(cmd_name, "Right") == 0)
        add_command_with_time(player, original_command, CMD_RIGHT_TIME);
    if (strcmp(cmd_name, "Left") == 0)
        add_command_with_time(player, original_command, CMD_LEFT_TIME);
}

void add_command_with_time(player_t *player, const char *command, int time)
{
    printf("Adding command to queue: %s (%d ticks)\n", command, time);
    add_action_to_queue(player, command, time);
}
