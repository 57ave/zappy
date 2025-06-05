/*
** EPITECH PROJECT, 2025
** server
** File description:
** TCP server
*/

#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>

void create_server(server_t *server)
{
    server->nb_clients = 0;
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    server->addr.sin_port = htons(server->port);
    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->game_started = false;
    bind(server->fd, (struct sockaddr*) &server->addr, sizeof(server->addr));
    listen(server->fd, NB_CONNECTION);
    return;
}

static void add_client(int client_fd, server_t *server)
{
    for (int i = 1; i < (NB_CONNECTION + 1); i++) {
        if (server->pfds[i].fd == FD_NULL) {
            server->pfds[i].fd = client_fd;
            server->pfds[i].events = POLLIN;
            server->nb_clients += 1;
            return;
        }
    }
    write(client_fd, "Connection with server failed\n", 30);
    return;
}

void handle_client(size_t nb_clients, server_t *server)
{
    struct sockaddr_in c_addr;
    socklen_t addrl = sizeof(c_addr);
    int client_fd = 0;

    if (server->pfds[0].revents & POLLIN) {
        for (size_t i = 0; i < nb_clients; i++) {
            client_fd = accept(server->fd, (struct sockaddr*) &c_addr, &addrl);
            add_client(client_fd, server);
        }
    }
    return;
}

void reset_server_clients(server_t *server)
{
    server->pfds[0].fd = server->fd;
    server->pfds[0].events = POLLIN;
    for (int i = 1; i < (NB_CONNECTION + 1); i++) {
        server->pfds[i].fd = FD_NULL;
        server->pfds[i].events = POLLIN;
        server->pfds[i].revents = -1;
    }
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
        server->clients[i].fd = FD_NULL;
    }
}

int launch_server(server_t *server)
{
    size_t clients_connected = 0;

    reset_server_clients(server);
    while (1) {
        clients_connected = poll(server->pfds, server->nb_clients + 1, 1);
        if (server->nb_clients < NB_CONNECTION) {
            handle_client(clients_connected, server);
        }
        // if (server->nb_clients == NB_CONNECTION) {
        //     read_client(server);
        // }
    }
    return SUCCESS;
}
