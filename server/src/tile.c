/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

bool add_tile_to_response(response_context_t *ctx,
    const char *tile_content)
{
    if (!*ctx->first_tile)
        strcat(ctx->response, ",");
    strcat(ctx->response, tile_content);
    return true;
}

char *create_element_string(const char *name, int value)
{
    size_t needed_size = strlen(name) + 20;
    char *element = malloc(needed_size);

    if (!element)
        return NULL;
    snprintf(element, needed_size, "%s %d", name, value);
    return element;
}

bool add_element_to_response(response_context_t *ctx, const char *element)
{
    if (!ctx->first_tile)
        strcat(ctx->response, ", ");
    strcat(ctx->response, element);
    ctx->first_tile = false;
    return true;
}

bool add_tile_players(tile_context_t *tile_ctx, response_context_t *resp_ctx)
{
    int player_count = count_players_on_tile(tile_ctx->server,
                            tile_ctx->pos.x, tile_ctx->pos.y);
    char *element;

    if (player_count <= 0)
        return true;
    element = create_element_string("player", player_count);
    if (!element)
        return false;
    add_element_to_response(resp_ctx, element);
    free(element);
    return true;
}

bool add_tile_resources(tile_context_t *tile_ctx, response_context_t *resp_ctx)
{
    tile_t *tile =
        &tile_ctx->server->map->tiles[tile_ctx->pos.y][tile_ctx->pos.x];
    char *element;

    for (int r = 0; r < RESOURCE_COUNT; r++) {
        if (tile->resources[r] <= 0)
            continue;
        element = create_element_string(get_resource_name(r),
                    tile->resources[r]);
        if (!element)
            return false;
        add_element_to_response(resp_ctx, element);
        free(element);
    }
    return true;
}
