/*
** EPITECH PROJECT, 2025
** handle client
** File description:
** handle_client
*/
#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int read_client_data(server_t *server, int i, char *buffer,
    size_t buffer_size)
{
    int read_size = 0;

    if (server->pfds[i].fd == FD_NULL) {
        fprintf(stderr, "Invalid Reading %d\n", server->pfds[i].fd);
        return -1;
    }
    read_size = read(server->pfds[i].fd, buffer, buffer_size - 1);
    if (read_size <= 0) {
        perror("Error in reading");
        close(server->pfds[i].fd);
        server->pfds[i].fd = FD_NULL;
        return -1;
    }
    buffer[read_size] = '\0';
    printf("Message de fd %d : %s", server->pfds[i].fd, buffer);
    return read_size;
}

team_t *find_team(const char *name, server_config_t *config)
{
    for (int i = 0; i < config->team_nb; i++) {
        if (strcmp(config->teams[i].name, name) == 0)
            return &config->teams[i];
    }
    return NULL;
}

void handle_team_command(server_t *server, server_config_t *config,
    int client_index, const char *buffer)
{
    int fd = server->pfds[client_index].fd;
    char team_name[256] = {0};
    team_t *team = NULL;

    strncpy(team_name, buffer + 5, sizeof(team_name) - 1);
    team_name[strcspn(team_name, "\n")] = '\0';
    team = find_team(team_name, config);
    if (!team) {
        write(fd, "ko\n", 3);
        return;
    }
    if (team->actual_players >= team->max_players) {
        write(fd, "ko\n", 3);
        return;
    }
    team->actual_players++;
    server->clients[client_index].type = CLIENT_IA;
    write(fd, "WELCOME\n", 8);
}

void handle_client_message(server_t *server, int i, const char *buffer,
    server_config_t *config)
{
    if (strncmp(buffer, "GRAPHIC", 7) == 0) {
        server->clients[i].type = CLIENT_GUI;
        write(server->pfds[i].fd, "WELCOME\n", 8);
    } else if (strncmp(buffer, "TEAM", 4) == 0) {
        handle_team_command(server, config, i, buffer);
    } else {
        write(server->pfds[i].fd, "ko\n", 3);
    }
}
