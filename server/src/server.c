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
    server->pfds[0].fd = server->fd;
    for (int i = 1; i < NB_CONNECTION; i++) {
        server->pfds[i].fd = FD_NULL;
        server->pfds[i].events = POLLIN;
    }
    bind(server->fd, (struct sockaddr*) &server->addr, sizeof(server->addr));
    listen(server->fd, NB_CONNECTION);
    return;
}

static void add_client(int client_fd, server_t *server)
{
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
        if (server->pfds[i].fd == FD_NULL) {
            server->pfds[i].fd = client_fd;
            server->pfds[i].events = POLLIN;
            server->nb_clients += 1;
            return;
        }
    }
    close(client_fd);
}

void handle_client(server_t *server)
{
    struct sockaddr_in c_addr;
    socklen_t addrl = sizeof(c_addr);
    int client_fd = accept(server->fd, (struct sockaddr*) &c_addr, &addrl);

    if (client_fd < 0) {
        perror("Error accept");
        return;
    }
    add_client(client_fd, server);
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

static int read_client_data(server_t *server, int i, char *buffer,
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

static void handle_client_message(server_t *server, int i, const char *buffer)
{
    if (strncmp(buffer, "GRAPHIC", 7) == 0) {
        write(server->pfds[i].fd, "WELCOME\n", 8);
    } else if (strncmp(buffer, "TEAM", 4) == 0) {
        write(server->pfds[i].fd, "WELCOME\n", 8);
        // TODO : appeler une fonction pour gérer les infos envoyées par l'IA
    } else {
        write(server->pfds[i].fd, "ko\n", 3);
    }
}

void read_client(server_t *server, int i)
{
    char buffer[1024] = {0};

    if (server->pfds[i].revents & POLLIN) {
        if (read_client_data(server, i, buffer, sizeof(buffer)) > 0) {
            handle_client_message(server, i, buffer);
        }
    }
}

int launch_server(server_t *server)
{
    size_t clients_connected = 0;

    reset_server_clients(server);
    while (1) {
        clients_connected = poll(server->pfds, NB_CONNECTION + 1, -1);
        if (clients_connected < 0) {
            perror("Erreur poll");
            continue;
        }
        if (server->pfds[0].revents & POLLIN) {
            handle_client(server);
        }
        for (int i = 1; i < NB_CONNECTION + 1; i++) {
            read_client(server, i);
        }
    }
    return SUCCESS;
}
