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

static int should_update_game(struct timeval *last_tick, long tick_interval)
{
    struct timeval current_time;
    long elapsed;

    gettimeofday(&current_time, NULL);
    elapsed = (current_time.tv_sec - last_tick->tv_sec) * 1000000 + 
              (current_time.tv_usec - last_tick->tv_usec);
    if (elapsed >= tick_interval) {
        *last_tick = current_time;
        return 1;
    }
    return 0;
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

static void server_main_loop(server_t *server, server_config_t *config,
    long tick_interval)
{
    int clients_connected;
    struct timeval last_tick;

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
        if (should_update_game(&last_tick, tick_interval))
            update_game_state(server);
    }
}

int launch_server(server_t *server, server_config_t *config)
{
    long tick_interval = 1000000 / config->freq;

    server->config = config;
    reset_server_clients(server);
    display_server_info(config);
    server_main_loop(server, config, tick_interval);
    return SUCCESS;
}