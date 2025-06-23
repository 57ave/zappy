/*
** EPITECH PROJECT, 2025
** map.c
** File description:
** map
*/
#include "map.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

float get_resource_density(resource_type_t type)
{
    switch (type) {
        case FOOD:
            return 0.5f;
        case LINEMATE:
            return 0.3f;
        case DERAUMERE:
            return 0.15f;
        case SIBUR:
            return 0.1f;
        case MENDIANE:
            return 0.1f;
        case PHIRAS:
            return 0.08f;
        case THYSTAME:
            return 0.05f;
        default:
            return 0.0f;
    }
}

void init_map(map_t *map, int width, int height)
{
    map->width = width;
    map->height = height;
    map->tiles = malloc(sizeof(tile_t *) * height);
    if (!map->tiles) {
        return;
    }
    for (int y = 0; y < height; y++) {
        map->tiles[y] = calloc(width, sizeof(tile_t));
    }
}

void generate_resources(map_t *map)
{
    int quantity = 0;
    int x = 0;
    int y = 0;

    srand(time(NULL));
    for (int type = 0; type < RESOURCE_COUNT; type++) {
        quantity = map->width * map->height * get_resource_density(type);
        for (int i = 0; i < quantity; i++) {
            x = rand() % map->width;
            y = rand() % map->height;
            map->tiles[y][x].resources[type]++;
            map->tiles[y][x].changed = true;
        }
    }
}

void free_map(map_t *map)
{
    for (int y = 0; y < map->height; y++) {
        free(map->tiles[y]);
    }
    free(map->tiles);
    free(map);
}
