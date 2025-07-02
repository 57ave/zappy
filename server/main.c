/*
** EPITECH PROJECT, 2025
** main.c
** File description:
** main
*/

#include "server.h"
#include <stdio.h>

static int check_arguments(int ac)
{
    if (ac < 9) {
        fprintf(stderr, "USAGE: ./zappy_server -p port -x width -y height");
        fprintf(stderr, " -n team1 team2 ... -c clientsNb -f freq\n");
        return FAILURE;
    }
    return SUCCESS;
}

static void cleanup_server(server_t *server)
{
    if (!server) {
        return;
    }
    if (server->map) {
        free_map(server->map);
    }
    free(server);
}

int validate_config(const server_config_t *config)
{
    if (config->port <= 0) {
        printf("Error: Invalid port\n");
        return -1;
    }
    if (config->nb_clients <= 0) {
        printf("Error: Number of clients must be > 0\n");
        return -1;
    }
    if (config->freq <= 0) {
        printf("Error: Frequency must be > 0\n");
        return -1;
    }
    if (config->team_nb <= 0 || config->team_name == NULL) {
        printf("Error: At least one team must be provided\n");
        return -1;
    }
    return 0;
}

int main(int ac, char **av)
{
    server_t *server = malloc(sizeof(server_t));
    server_config_t config = {0};

    if (!server || check_arguments(ac) != SUCCESS) {
        return FAILURE;
    }
    if (parse_args(ac, av, &config) < 0 || validate_config(&config) < 0) {
        return FAILURE;
    }
    server->port = config.port;
    create_server(server);
    server->map = malloc(sizeof(map_t));
    init_map(server->map, config.width, config.height);
    generate_resources(server->map);
    if (launch_server(server, &config))
        return FAILURE;
    cleanup_server(server);
    return SUCCESS;
}
