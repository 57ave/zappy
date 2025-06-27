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

static void update_all_players_life(server_t *server)
{
    for (int i = 0; i < server->player_nb; i++) {
        update_single_player_life(server->players[i]);
    }
}

void update_game_state(server_t *server)
{
    update_player_actions(server);
    process_completed_actions(server);
    update_all_players_life(server);
}

static void cleanup_disconnected_client(server_t *server, int i)
{
    printf("Client %d disconnected (fd=%d)\n", i, server->pfds[i].fd);
    close(server->pfds[i].fd);
    server->pfds[i].fd = FD_NULL;
    server->clients[i].fd = FD_NULL;
    server->clients[i].type = -1;
    server->clients[i].player = NULL;
    server->nb_clients--;
}

void remove_disconnected_clients(server_t *server)
{
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
        if (server->pfds[i].fd == FD_NULL)
            continue;
        if (server->pfds[i].revents & POLLHUP)
            cleanup_disconnected_client(server, i);
    }
}

void process_new_connections(server_t *server)
{
    if (server->pfds[0].revents & POLLIN)
        handle_client(server);
}
