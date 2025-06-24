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
    #include "player.h"
    #include "map.h"

    #define NB_CONNECTION 12
    #define MAX_PLAYERS 130
    #define FD_NULL -1
    #define SUCCESS 0
    #define FAILURE 84

typedef struct server_config_s server_config_t;

typedef struct server_s {
    int fd;
    int port;
    int nb_clients;
    bool game_started;
    struct sockaddr_in addr;
    map_t *map;
    player_t *players[MAX_PLAYERS];
    int player_nb;
    struct pollfd pfds[NB_CONNECTION + 1];
    client_t clients[NB_CONNECTION + 1];
    server_config_t *config; // Référence vers la configuration
} server_t;

struct server_config_s {
    int port;
    int width;
    int height;
    char **team_name;
    int team_nb;
    int nb_clients;
    int freq;
    int tick_freq;
    team_t *teams;
};

// Fonctions du serveur
void create_server(server_t *serv);
void handle_client(server_t *serv);
int launch_server(server_t *serv, server_config_t *config);
void reset_server_clients(server_t *serv);

// Fonctions de parsing
int parse_args(int ac, char **av, server_config_t *config);

// Fonctions de gestion des clients
int read_client_data(server_t *server, int i, char *buffer, size_t buffer_size);
void handle_client_message(server_t *server, int i, const char *buffer,
    server_config_t *config);
void send_data_gui(server_t *server, int gui_fd, server_config_t *config);
void update_player_life(server_t *server);
void send_gui_resource_changes(server_t *server);

// Fonctions utilitaires
team_t *find_team(const char *name, server_config_t *config);

#endif /* !SERVER_H_ */