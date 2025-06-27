/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

bool process_single_tile(process_context_t *ctx, look_params_t params)
{
    position_t pos = calculate_look_coordinates(ctx->player,
        ctx->server, params);
    tile_context_t tile_ctx = {ctx->server, pos};

    if (!add_tile_resources(&tile_ctx, ctx->resp_ctx))
        return false;
    if (!add_tile_players(&tile_ctx, ctx->resp_ctx))
        return false;
    return true;
}

bool process_vision_level(process_context_t *ctx, int level)
{
    look_params_t params;

    params.level = level;
    for (int offset = -level; offset <= level; offset++) {
        params.offset = offset;
        if (!process_single_tile(ctx, params))
            return false;
    }
    return true;
}

char *build_look_response(server_t *server, player_t *player)
{
    size_t response_size = calculate_response_size(server, player);
    char *response = malloc(response_size);
    bool first_item = true;
    int vision_range = player->lvl;
    response_context_t resp_ctx = {response, &first_item};
    process_context_t proc_ctx = {server, player, &resp_ctx};

    if (!response)
        return NULL;
    strcpy(response, "[");
    for (int level = 0; level <= vision_range; level++) {
        if (!process_vision_level(&proc_ctx, level)) {
            free(response);
            return NULL;
        }
    }
    strcat(response, "]");
    return response;
}

void cmd_look(server_t *server, player_t *player)
{
    char *response = build_look_response(server, player);

    if (!response) {
        dprintf(player->fd, "ko\n");
        return;
    }
    dprintf(player->fd, "%s\n", response);
    free(response);
}

