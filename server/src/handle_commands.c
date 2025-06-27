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

static char *create_clean_command_copy(const char *buffer)
{
    char *command_copy = NULL;
    char *newline_pos = NULL;
    size_t len = strlen(buffer) + 1;

    command_copy = malloc(len);
    if (!command_copy)
        return NULL;
    strncpy(command_copy, buffer, len - 1);
    command_copy[len - 1] = '\0';
    newline_pos = strchr(command_copy, '\n');
    if (newline_pos)
        *newline_pos = '\0';
    return command_copy;
}

static void handle_ia_command(server_t *server, int i, const char *buffer)
{
    char *command_copy = create_clean_command_copy(buffer);

    if (!command_copy) {
        printf("Memory allocation failed for IA command\n");
        write(server->pfds[i].fd, "ko\n", 3);
        return;
    }
    printf("Executing IA command: '%s' for player %d\n", 
           command_copy, server->clients[i].player->id);
    execute_command(server, server->clients[i].player, command_copy);
    free(command_copy);
}

static void handle_gui_command(const char *buffer)
{
    printf("GUI command received: %s", buffer);
}

static void handle_unknown_client_state(server_t *server, int i)
{
    printf("Unknown client state: type=%d\n", server->clients[i].type);
    write(server->pfds[i].fd, "ko\n", 3);
}

void handle_client_message(server_t *server, int i, const char *buffer,
    server_config_t *config)
{
    printf("Client %d (type=%d): received '%s'\n",
        i, server->clients[i].type, buffer);
    if (strncmp(buffer, "GRAPHIC", 7) == 0) {
        handle_graphic_client_registration(server, i);
        return;
    }
    if (server->clients[i].type == -1) {
        handle_team_command(server, config, i, buffer);
        return;
    }
    if (server->clients[i].type == CLIENT_IA && server->clients[i].player) {
        handle_ia_command(server, i, buffer);
        return;
    }
    if (server->clients[i].type == CLIENT_GUI) {
        handle_gui_command(buffer);
        return;
    }
    handle_unknown_client_state(server, i);
}
