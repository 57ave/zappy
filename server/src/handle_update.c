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

static char *create_clean_team_name(const char *buffer)
{
    char *team_name = NULL;
    size_t len = strlen(buffer) + 1;
    char *newline_pos = NULL;

    team_name = malloc(len);
    if (!team_name)
        return NULL;
    strncpy(team_name, buffer, len - 1);
    team_name[len - 1] = '\0';
    newline_pos = strchr(team_name, '\n');
    if (newline_pos)
        *newline_pos = '\0';
    return team_name;
}

static int validate_team_availability(team_t *team, const char *team_name, int fd)
{
    if (!team) {
        printf("Team '%s' not found\n", team_name);
        write(fd, "ko\n", 3);
        return 0;
    }
    if (team->actual_players >= team->max_players) {
        printf("Team '%s' is full\n", team_name);
        write(fd, "ko\n", 3);
        return 0;
    }
    return 1;
}

void handle_team_command(server_t *server, server_config_t *config,
    int client_index, const char *buffer)
{
    int fd = server->pfds[client_index].fd;
    char *team_name = create_clean_team_name(buffer);
    team_t *team = NULL;

    if (!team_name) {
        printf("Memory allocation failed for team name\n");
        write(fd, "ko\n", 3);
        return;
    }
    printf("Team command received: '%s'\n", team_name);
    team = find_team(team_name, config);
    if (validate_team_availability(team, team_name, fd)) {
        register_player(server, client_index, team, team_name);
    }
    free(team_name);
}

void handle_graphic_client_registration(server_t *server, int i)
{
    server->clients[i].type = CLIENT_GUI;
    write(server->pfds[i].fd, "WELCOME\n", 8);
    printf("Client %d registered as GUI\n", i);
}