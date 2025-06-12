/*
** EPITECH PROJECT, 2025
** server.h
** File description:
** server
*/

#ifndef SERVER_H_
    #define SERVER_H_
    #include <poll.h>
    #include <errno.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include "client.h"
    #include "map.h"

    #define NB_CONNECTION 12
    #define FD_NULL -1
    #define SUCCESS 0
    #define FAILURE 84

typedef struct {
    int fd;
    int port;
    int nb_clients;
    bool game_started;
    struct sockaddr_in addr;
    map_t *map;
    struct pollfd pfds[NB_CONNECTION + 1];
    client_t clients[NB_CONNECTION + 1];
} server_t;

typedef struct {
    int port;
    int width;
    int height;
    char **team_name;
    int team_nb;
    int nb_clients;
    int freq;
} server_config_t;

void create_server(server_t *serv);
void handle_client(server_t *serv);
int launch_server(server_t *serv);
void reset_server_clients(server_t *serv);
int parse_args(int ac, char **av, server_config_t *config);

#endif /* !SERVER_H_ */
