/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

char *create_resource_info(int resource_index, int quantity)
{
    const char *resource_name = get_resource_name(resource_index);
    size_t needed_size = strlen(resource_name) + 20;
    char *resource_info = malloc(needed_size);
    
    if (!resource_info)
        return NULL;
    
    snprintf(resource_info, needed_size, "%s %d", resource_name, quantity);
    return resource_info;
}

bool add_single_resource(tile_t *tile, char *tile_content, bool *first_item, int r)
{
    char *resource_info;
    if (tile->resources[r] <= 0)
        return true;
    
    if (!*first_item)
        strcat(tile_content, " ");
    
    resource_info = create_resource_info(r, tile->resources[r]);
    if (!resource_info)
        return false;
    
    strcat(tile_content, resource_info);
    free(resource_info);
    *first_item = false;
    return true;
}

void add_resources_to_tile_content(tile_t *tile, char *tile_content, bool *first_item)
{
    for (int r = 0; r < RESOURCE_COUNT; r++) {
        if (!add_single_resource(tile, tile_content, first_item, r))
            return;
    }
}

char *create_player_info(int player_count)
{
    size_t needed_size = 20;
    char *player_info = malloc(needed_size);
    
    if (!player_info)
        return NULL;
    
    snprintf(player_info, needed_size, "player %d", player_count);
    return player_info;
}

void add_players_to_tile_content(int player_count, char *tile_content, bool *first_item)
{
    char *player_info;

    if (player_count <= 0)
        return;
    
    if (!*first_item)
        strcat(tile_content, " ");
    
    player_info = create_player_info(player_count);
    if (!player_info)
        return;
    
    strcat(tile_content, player_info);
    free(player_info);
}