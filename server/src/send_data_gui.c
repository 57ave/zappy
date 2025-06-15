/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** send data to gui
*/
#include "map.h"
#include "server.h"

void send_map_size_to_gui(int gui_fd, map_t *map)
{
    dprintf(gui_fd, "msz %d %d\n", map->width, map->height);
}

void send_map_content_to_gui(int gui_fd, map_t *map)
{
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            tile_t tile = map->tiles[y][x];
            dprintf(gui_fd, "bct %d %d %d %d %d %d %d %d %d\n",
                x, y,
                tile.resources[FOOD],
                tile.resources[LINEMATE],
                tile.resources[DERAUMERE],
                tile.resources[SIBUR],
                tile.resources[MENDIANE],
                tile.resources[PHIRAS],
                tile.resources[THYSTAME]);
        }
    }
}