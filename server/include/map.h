/*
** EPITECH PROJECT, 2025
** map.h
** File description:
** map
*/

#ifndef MAP_H_
    #define MAP_H_
typedef enum {
    FOOD,
    LINEMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PHIRAS,
    THYSTAME,
    RESOURCE_COUNT
} resource_type_t;

typedef struct {
    int resources[RESOURCE_COUNT];
} tile_t;

typedef struct {
    int width;
    int height;
    tile_t **tiles;
} map_t;

float get_resource_density(resource_type_t type);
void init_map(map_t *map, int width, int height);
void generate_resources(map_t *map);
void free_map(map_t *map);
#endif /* !MAP_H_ */
