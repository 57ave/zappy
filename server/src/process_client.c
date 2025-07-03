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

int handle_tick(struct timeval *last_tick, server_config_t *config)
{
    struct timeval now = {0};
    long elapsed_usec = 0;

    gettimeofday(&now, NULL);
    elapsed_usec = (now.tv_sec - last_tick->tv_sec) * 1000000
        + (now.tv_usec - last_tick->tv_usec);
    if (elapsed_usec >= config->tick_freq) {
        *last_tick = now;
        return 1;
    }
    return 0;
}

void process_clients(server_t *server, server_config_t *config,
    int clients_connected)
{
    if (clients_connected <= 0) {
        return;
    }
    if (server->pfds[0].revents & POLLIN) {
        handle_client(server);
    }
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
        read_client(server, config, i);
    }
}
