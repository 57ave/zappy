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

int main(int ac, char **av)
{
    server_t *server = malloc(sizeof(server_t));
    server_config_t config = {0};

    if (!server || check_arguments(ac) != SUCCESS) {
        return FAILURE;
    }
    if (parse_args(ac, av, &config) < 0) {
        return FAILURE;
    }
    if (config.freq <= 0)
        return FAILURE;
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
