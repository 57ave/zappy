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

char *format_inventory_item(int resource_index, int quantity)
{
    const char *resource_name = get_resource_name(resource_index);
    int needed_size = strlen(resource_name) + 20;
    char *item = malloc(needed_size);
    
    if (!item)
        return NULL;
    
    snprintf(item, needed_size, "%s %d", resource_name, quantity);
    return item;
}

size_t calculate_item_size(int resource_index, int quantity)
{
    const char *resource_name = get_resource_name(resource_index);
    return strlen(resource_name) + 20;
}

size_t calculate_total_size(player_t *player)
{
    size_t total_size = 3;
    int item_count = 0;
    
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (player->inventory[i] <= 0)
            continue;
        
        total_size += calculate_item_size(i, player->inventory[i]);
        if (item_count > 0)
            total_size += 2;
        item_count++;
    }
    
    return total_size;
}

void add_item_to_response(char *response, const char *item, bool first)
{
    if (!first)
        strcat(response, ", ");
    strcat(response, item);
}

bool process_single_item(player_t *player, int i, char *response, bool *first)
{
    if (player->inventory[i] <= 0)
        return true;
    
    char *item = format_inventory_item(i, player->inventory[i]);
    if (!item)
        return false;
    
    add_item_to_response(response, item, *first);
    free(item);
    *first = false;
    return true;
}