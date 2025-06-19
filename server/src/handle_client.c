/*
** EPITECH PROJECT, 2025
** handle client
** File description:
** handle_client
*/
#include "server.h"
#include "commands.h"
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

static void register_player(server_t *server, int client_index,
    team_t *team, const char *team_name)
{
    int fd = server->pfds[client_index].fd;
    int available_slot = 0;
    player_t *player = create_player(server->player_nb, fd, team_name,
        server->map);

    if (!player) {
        write(fd, "ko\n", 3);
        return;
    }
    server->players[server->player_nb] = player;
    server->player_nb++;
    server->clients[client_index].type = CLIENT_IA;
    server->clients[client_index].player = player;
    team->actual_players++;
    available_slot = team->max_players - team->actual_players;
    dprintf(fd, "%d\n", available_slot);
    dprintf(fd, "%d %d\n", server->map->width, server->map->height);
}

void handle_team_command(server_t *server, server_config_t *config,
    int client_index, const char *buffer)
{
    int fd = server->pfds[client_index].fd;
    char team_name[256] = {0};
    team_t *team = NULL;

    strncpy(team_name, buffer, sizeof(team_name) - 1);
    team_name[strcspn(team_name, "\n")] = '\0';
    team = find_team(team_name, config);
    if (!team || team->actual_players >= team->max_players) {
        write(fd, "ko\n", 3);
        return;
    }
    register_player(server, client_index, team, team_name);
}

void handle_client_message(server_t *server, int i, const char *buffer,
    server_config_t *config)
{
    // Si le client n'est pas encore identifié
    if (server->clients[i].type != CLIENT_IA && server->clients[i].type != CLIENT_GUI) {
        if (strncmp(buffer, "GRAPHIC", 7) == 0) {
            server->clients[i].type = CLIENT_GUI;
            write(server->pfds[i].fd, "WELCOME\n", 8);
        } else {
            handle_team_command(server, config, i, buffer);
        }
        return;
    }
    
    // Si c'est un client IA, traiter les commandes
    if (server->clients[i].type == CLIENT_IA && server->clients[i].player) {
        char command_copy[1024];
        strncpy(command_copy, buffer, sizeof(command_copy) - 1);
        command_copy[sizeof(command_copy) - 1] = '\0';
        execute_command(server, server->clients[i].player, command_copy);
    }
    
    // Si c'est un client GUI, traiter les commandes GUI (à implémenter)
    if (server->clients[i].type == CLIENT_GUI) {
        // Traitement des commandes GUI à implémenter plus tard
        printf("GUI command received: %s", buffer);
    }
}