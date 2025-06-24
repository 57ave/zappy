/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int count_players_on_tile(server_t *server, int x, int y)
{
    int player_count = 0;

    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i]->x == x && server->players[i]->y == y)
            player_count++;
    }
    return player_count;
}

size_t calculate_resource_size(int resource_index, int quantity, bool need_separator)
{
    size_t size = strlen(get_resource_name(resource_index)) + 20; // nom + nombre + espace
    if (need_separator)
        size += 1;
    return size;
}

size_t calculate_element_size(const char *name, int value)
{
    return strlen(name) + 20;
}

size_t calculate_tile_content_size(tile_context_t *ctx)
{
    tile_t *tile = &ctx->server->map->tiles[ctx->pos.y][ctx->pos.x];
    size_t size = 0;
    int player_count = count_players_on_tile(ctx->server, ctx->pos.x, ctx->pos.y);
    
    for (int r = 0; r < RESOURCE_COUNT; r++) {
        if (tile->resources[r] > 0)
            size += calculate_element_size(get_resource_name(r), tile->resources[r]);
    }
    
    if (player_count > 0)
        size += calculate_element_size("player", player_count);
    
    return size;
}

void build_tile_content(server_t *server, position_t pos, char *tile_content)
{
    tile_t *tile = &server->map->tiles[pos.y][pos.x];
    bool first_item = true;
    int player_count = count_players_on_tile(server, pos.x, pos.y);

    strcpy(tile_content, "");
    add_resources_to_tile_content(tile, tile_content, &first_item);
    add_players_to_tile_content(player_count, tile_content, &first_item);
}