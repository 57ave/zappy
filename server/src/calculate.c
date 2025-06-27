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
    build_tile_content(ctx->server, ctx->pos, tile_content);
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

size_t calculate_response_size(server_t *server, player_t *player)
{
    int vision_range = player->lvl;
    size_t total_size = 3;
    look_params_t params;
    int total_elements = 0;
    position_t pos;
    tile_context_t ctx;

    for (int level = 0; level <= vision_range; level++) {
        for (int offset = -level; offset <= level; offset++) {
            params.level = level;
            params.offset = offset;
            pos = calculate_look_coordinates(player, server, params);
            ctx.server = server;
            ctx.pos = pos;
            total_size += calculate_tile_content_size(&ctx);
        }
    }
    total_elements = calculate_total_elements(server, player);
    if (total_elements > 1)
        total_size += (total_elements - 1) * 2;
    return total_size;
}
