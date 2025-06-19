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

void create_server(server_t *server)
{
    server->nb_clients = 0;
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    server->addr.sin_port = htons(server->port);
    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->game_started = false;
    server->player_nb = 0;
    server->pfds[0].fd = server->fd;
    server->pfds[0].events = POLLIN;
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
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
            server->clients[i].fd = client_fd;
            server->clients[i].type = -1; // Non identifié
            server->clients[i].player = NULL;
            write(client_fd, "WELCOME\n", 8);
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
        server->clients[i].type = -1;
        server->clients[i].player = NULL;
    }
}

void read_client(server_t *server, server_config_t *config, int i)
{
    char buffer[1024] = {0};

    if (server->pfds[i].revents & POLLIN) {
        if (read_client_data(server, i, buffer, sizeof(buffer)) > 0) {
            handle_client_message(server, i, buffer, config);
        }
    }
}

static void update_game_state(server_t *server)
{
    // Mettre à jour les actions des joueurs
    update_player_actions(server);
    
    // Traiter les actions terminées
    process_completed_actions(server);
    
    // Mettre à jour la vie des joueurs (consommation de nourriture)
    for (int i = 0; i < server->player_nb; i++) {
        player_t *player = server->players[i];
        player->life_remain--;
        
        if (player->life_remain <= 0) {
            if (player->inventory[FOOD] > 0) {
                player->inventory[FOOD]--;
                player->life_remain = 1260; // Reset life
            } else {
                // Le joueur meurt
                dprintf(player->fd, "dead\n");
                close(player->fd);
                // Retirer le joueur du serveur (à implémenter)
            }
        }
    }
}

int launch_server(server_t *server, server_config_t *config)
{
    int clients_connected = 0;
    struct timeval last_tick, current_time;
    long tick_interval = 1000000 / config->freq; // En microsecondes
    
    server->config = config;
    reset_server_clients(server);
    gettimeofday(&last_tick, NULL);
    
    while (1) {
        // Poll avec timeout pour le game tick
        clients_connected = poll(server->pfds, NB_CONNECTION + 1, 50);
        
        if (clients_connected < 0) {
            perror("Erreur poll");
            continue;
        }
        
        // Traiter les nouvelles connexions
        if (server->pfds[0].revents & POLLIN) {
            handle_client(server);
        }
        
        // Traiter les messages des clients
        for (int i = 1; i < NB_CONNECTION + 1; i++) {
            if (server->pfds[i].fd != FD_NULL) {
                read_client(server, config, i);
            }
        }
        
        // Mettre à jour l'état du jeu selon la fréquence
        gettimeofday(&current_time, NULL);
        long elapsed = (current_time.tv_sec - last_tick.tv_sec) * 1000000 + 
                      (current_time.tv_usec - last_tick.tv_usec);
        
        if (elapsed >= tick_interval) {
            update_game_state(server);
            last_tick = current_time;
        }
    }
    return SUCCESS;
}
