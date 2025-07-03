/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

int count_players_on_tile(server_t *server, int x, int y)
{
    int player_count = 0;

    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i]->x == x && server->players[i]->y == y)
            player_count++;
    }
    return player_count;
}

size_t calculate_element_size(const char *name, int value)
{
    (void)value;
    return strlen(name) + 20;
}

size_t calculate_tile_content_size(tile_context_t *ctx)
{
    tile_t *tile = &ctx->server->map->tiles[ctx->pos.y][ctx->pos.x];
    size_t size = 0;
    int player_count = count_players_on_tile(ctx->server, ctx->pos.x,
        ctx->pos.y);

    for (int r = 0; r < RESOURCE_COUNT; r++) {
        if (tile->resources[r] > 0)
            size += calculate_element_size(get_resource_name(r),
                tile->resources[r]);
    }
    if (player_count > 0)
        size += calculate_element_size("player", player_count);
    return size;
}

static void append_tile_resources(tile_t *tile, char *tile_str,
    size_t size)
{
    char buffer[64];

    for (int r = 0; r < RESOURCE_COUNT; r++) {
        if (tile->resources[r] > 0) {
            snprintf(buffer, sizeof(buffer), "%s %d ",
                get_resource_name(r), tile->resources[r]);
            strncat(tile_str, buffer, size - strlen(tile_str) - 1);
        }
    }
}

static void append_tile_players(server_t *server, position_t pos,
    char *tile_str, size_t size)
{
    char buffer[64];
    int player_count = count_players_on_tile(server, pos.x, pos.y);

    if (player_count > 0) {
        snprintf(buffer, sizeof(buffer), "player %d ", player_count);
        strncat(tile_str, buffer, size - strlen(tile_str) - 1);
    }
}

bool build_tile_content(tile_context_t *tile_ctx, char *tile_str,
    size_t size)
{
    tile_t *tile =
        &tile_ctx->server->map->tiles[tile_ctx->pos.y][tile_ctx->pos.x];
    size_t len = 0;

    tile_str[0] = '\0';
    append_tile_resources(tile, tile_str, size);
    append_tile_players(tile_ctx->server, tile_ctx->pos, tile_str, size);
    len = strlen(tile_str);
    if (len > 0 && tile_str[len - 1] == ' ')
        tile_str[len - 1] = '\0';
    return true;
}
