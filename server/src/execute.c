/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

static void handle_info_commands(server_t *server, player_t *player,
    const char *cmd_name, const char *args, const char *original_command)
{
    (void)args;
    if (strcmp(cmd_name, "Look") == 0)
        add_command_with_time(player, original_command, CMD_LOOK_TIME);
    if (strcmp(cmd_name, "Inventory") == 0)
        add_command_with_time(player, original_command, CMD_INVENTORY_TIME);
    if (strcmp(cmd_name, "Connect_nbr") == 0)
        cmd_connect_nbr(server, player);
}

static void handle_action_commands(player_t *player, const char *cmd_name,
    const char *original_command)
{
    if (strcmp(cmd_name, "Broadcast") == 0)
        add_command_with_time(player, original_command, CMD_BROADCAST_TIME);
    if (strcmp(cmd_name, "Fork") == 0)
        add_command_with_time(player, original_command, CMD_FORK_TIME);
    if (strcmp(cmd_name, "Eject") == 0)
        add_command_with_time(player, original_command, CMD_EJECT_TIME);
    if (strcmp(cmd_name, "Take") == 0)
        add_command_with_time(player, original_command, CMD_TAKE_TIME);
    if (strcmp(cmd_name, "Set") == 0)
        add_command_with_time(player, original_command, CMD_SET_TIME);
    if (strcmp(cmd_name, "Incantation") == 0)
        add_command_with_time(player, original_command, CMD_INCANTATION_TIME);
}

static int is_valid_command(const char *cmd_name)
{
    const char *valid_commands[] = {
        "Forward", "Right", "Left", "Look", "Inventory", "Broadcast",
        "Connect_nbr", "Fork", "Eject", "Take", "Set", "Incantation", NULL
    };

    for (int i = 0; valid_commands[i]; i++) {
        if (strcmp(cmd_name, valid_commands[i]) == 0)
            return 1;
    }
    return 0;
}

static char *clean_command_copy(const char *original)
{
    char *copy = create_command_copy(original);
    char *newline_pos;

    if (!copy)
        return NULL;
    newline_pos = strchr(copy, '\n');
    if (newline_pos)
        *newline_pos = '\0';
    return copy;
}

void execute_command(server_t *server, player_t *player, char *command)
{
    char *original_command = clean_command_copy(command);
    char *cmd_name = strtok(command, " \n");
    char *args = strtok(NULL, "\n");

    if (!cmd_name || !original_command) {
        dprintf(player->fd, "ko\n");
        free(original_command);
        return;
    }
    if (!is_valid_command(cmd_name)) {
        dprintf(player->fd, "ko\n");
        free(original_command);
        return;
    }
    handle_movement_commands(player, cmd_name, original_command);
    handle_info_commands(server, player, cmd_name, args, original_command);
    handle_action_commands(player, cmd_name, original_command);
    free(original_command);
}
