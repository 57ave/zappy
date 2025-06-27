/*
** EPITECH PROJECT, 2025
** Parsing arg
** File description:
** parse_arg
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include <ctype.h>

int parse_port(int i, int ac, char **av)
{
    if (i + 1 < ac) {
        return atoi(av[i + 1]);
    }
    return -1;
}

static int is_valid_int(const char *str)
{
    if (!str || *str == '\0')
        return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int parse_world_size(int i, int ac, char **av)
{
    if (i + 1 < ac && is_valid_int(av[i + 1])) {
        return atoi(av[i + 1]);
    }
    return -1;
}

char **parse_teams(int i, int ac, char **av, int *team_nb)
{
    int start = i + 1;
    char **teams = NULL;

    while (start < ac && av[start][0] != '-') {
        start++;
    }
    *team_nb = start - (i + 1);
    teams = malloc(*team_nb * sizeof(char *));
    for (int j = 0; j < *team_nb; j++) {
        teams[j] = av[i + 1 + j];
    }
    return teams;
}

int parse_begin(int ac, char **av, server_config_t *config, int i)
{
    if (strcmp(av[i], "-p") == 0) {
            config->port = parse_port(i, ac, av);
            i++;
    }
    if (strcmp(av[i], "-x") == 0) {
        config->width = parse_world_size(i, ac, av);
        if (config->width <= 0)
            return -1;
        i++;
        return i;
    }
    if (strcmp(av[i], "-y") == 0) {
        config->height = parse_world_size(i, ac, av);
        if (config->height <= 0)
            return -1;
        i++;
        return i;
    }
    return i;
}

static void init_teams(int ac, char **av, server_config_t *config, int i)
{
    if (strcmp(av[i], "-n") == 0) {
            config->team_name = parse_teams(i, ac, av, &config->team_nb);
            config->teams = malloc(sizeof(team_t) * config->team_nb);
            for (int j = 0; j < config->team_nb; j++) {
                config->teams[j].name = strdup(config->team_name[j]);
                config->teams[j].max_players = config->nb_clients;
                config->teams[j].actual_players = 0;
            }
        }
}

int parse_args(int ac, char **av, server_config_t *config)
{
    for (int i = 1; i < ac; i++) {
        i = parse_begin(ac, av, config, i);
        if (i == -1)
            return -1;
        if (strcmp(av[i], "-c") == 0) {
            config->nb_clients = parse_world_size(i, ac, av);
            i++;
        }
        if (strcmp(av[i], "-f") == 0) {
            config->freq = parse_world_size(i, ac, av);
            config->tick_freq = 1000000 / config->freq;
            i++;
        }
    }
    for (int i = 1; i < ac; i++) {
        init_teams(ac, av, config, i);
    }
    return 0;
}
