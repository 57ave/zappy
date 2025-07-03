/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

static position_t calculate_position_up(player_t *player, server_t *server,
    look_params_t params)
{
    position_t pos;

    pos.x = calcul_pos_add(player->x, params.offset, server->map->width);
    pos.y = calcul_pos_sub(player->y, params.level, server->map->height);
    return pos;
}

static position_t calculate_position_right(player_t *player, server_t *server,
    look_params_t params)
{
    position_t pos;

    pos.x = (player->x + params.level) % server->map->width;
    pos.y = calcul_pos_add(player->y, params.offset, server->map->height);
    return pos;
}

static position_t calculate_position_down(player_t *player, server_t *server,
    look_params_t params)
{
    position_t pos;

    pos.x = calcul_pos_sub(player->x, params.offset, server->map->width);
    pos.y = (player->y + params.level) % server->map->height;
    return pos;
}

static position_t calculate_position_left(player_t *player, server_t *server,
    look_params_t params)
{
    position_t pos;

    pos.x = calcul_pos_sub(player->x, params.level, server->map->width);
    pos.y = calcul_pos_sub(player->y, params.offset, server->map->height);
    return pos;
}

position_t calculate_look_coordinates(player_t *player, server_t *server,
    look_params_t params)
{
    if (player->dir == UP)
        return calculate_position_up(player, server, params);
    if (player->dir == RIGHT)
        return calculate_position_right(player, server, params);
    if (player->dir == DOWN)
        return calculate_position_down(player, server, params);
    if (player->dir == LEFT)
        return calculate_position_left(player, server, params);
    return (position_t){0, 0};
}
