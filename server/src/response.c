/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "server.h"
#include "commands.h"

void init_response_calculation(player_t *player, size_t *total_size,
    int *vision_range)
{
    *vision_range = player->lvl;
    *total_size = 3;
}

size_t process_vision_level_size(server_t *server, player_t *player,
    int level)
{
    size_t level_size = 0;
    look_params_t params;
    position_t pos;
    tile_context_t ctx;

    for (int offset = -level; offset <= level; offset++) {
        params.level = level;
        params.offset = offset;
        pos = calculate_look_coordinates(player, server, params);
        ctx.server = server;
        ctx.pos = pos;
        level_size += calculate_tile_content_size(&ctx);
    }
    return level_size;
}

size_t calculate_all_levels_size(server_t *server, player_t *player,
    int vision_range)
{
    size_t total_level_size = 0;

    for (int level = 0; level <= vision_range; level++) {
        total_level_size += process_vision_level_size(server, player, level);
    }
    return total_level_size;
}

size_t add_separator_size(server_t *server, player_t *player,
    size_t base_size)
{
    int total_elements = calculate_total_elements(server, player);

    if (total_elements > 1)
        return base_size + (total_elements - 1) * 2;
    return base_size;
}

size_t calculate_response_size(server_t *server, player_t *player)
{
    size_t total_size;
    int vision_range;

    init_response_calculation(player, &total_size, &vision_range);
    total_size += calculate_all_levels_size(server, player, vision_range);
    return add_separator_size(server, player, total_size);
}
