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

static void process_client_messages(server_t *server, server_config_t *config)
{
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
        if (server->pfds[i].fd != FD_NULL)
            read_client(server, config, i);
    }
}

static void display_server_info(server_config_t *config)
{
    printf("Server launched: port=%d, freq=%d, teams=%d\n",
        config->port, config->freq, config->team_nb);
    for (int i = 0; i < config->team_nb; i++) {
        printf("Team %d: %s (max_players=%d)\n",
            i, config->teams[i].name, config->teams[i].max_players);
    }
}

static void server_main_loop(server_t *server, server_config_t *config)
{
    int clients_connected;
    struct timeval last_tick;
    int tick_count = 0;

    gettimeofday(&last_tick, NULL);
    while (1) {
        clients_connected = poll(server->pfds, NB_CONNECTION + 1, 50);
        if (clients_connected < 0) {
                perror("Erreur poll");
            continue;
        }
        process_new_connections(server);
        process_client_messages(server, config);
        remove_disconnected_clients(server);
        handle_game_tick(server, config, &last_tick, &tick_count);
            
    }
}

int launch_server(server_t *server, server_config_t *config)
{
    server->config = config;
    reset_server_clients(server);
    display_server_info(config);
    server_main_loop(server, config);
    return SUCCESS;
}
