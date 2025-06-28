/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

char *create_tile_content(tile_context_t *ctx)
{
    size_t needed_size = calculate_tile_content_size(ctx);
    char *tile_content = malloc(needed_size);

    if (!tile_content)
        return NULL;
    if (!build_tile_content(ctx, tile_content, needed_size)) {
        free(tile_content);
        return NULL;
    }
    return tile_content;
}

int calculate_total_tiles(int vision_range)
{
    int total = 0;

    for (int level = 0; level <= vision_range; level++) {
        for (int offset = -level; offset <= level; offset++) {
            total++;
        }
    }
    return total;
}

int count_tile_elements(server_t *server, position_t pos)
{
    tile_t *tile = &server->map->tiles[pos.y][pos.x];
    int elements = 0;
    int player_count = 0;

    for (int r = 0; r < RESOURCE_COUNT; r++) {
        if (tile->resources[r] > 0)
            elements++;
    }
    player_count = count_players_on_tile(server, pos.x, pos.y);
    if (player_count > 0)
        elements++;
    return elements;
}

int calculate_total_elements(server_t *server, player_t *player)
{
    int vision_range = player->lvl;
    int total_elements = 0;
    look_params_t params;
    position_t pos;

    for (int level = 0; level <= vision_range; level++) {
        for (int offset = -level; offset <= level; offset++) {
            params.level = level;
            params.offset = offset;
            pos = calculate_look_coordinates(player, server, params);
            total_elements += count_tile_elements(server, pos);
        }
    }
    return total_elements;
}
