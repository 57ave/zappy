/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** send data to gui
*/
#include "map.h"
#include "server.h"
#include "stdio.h"
#include <stdarg.h>

void send_map_size_to_gui(int gui_fd, map_t *map)
{
    dprintf(gui_fd, "msz %d %d\n", map->width, map->height);
}

void send_map_content_to_gui(int gui_fd, map_t *map)
{
    tile_t tile;

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            tile = map->tiles[y][x];
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

static void send_players_to_gui(int gui_fd, server_t *server)
{
    player_t *player = NULL;

    for (int i = 0; i < server->player_nb; i++) {
        player = server->players[i];
        dprintf(gui_fd, "pnw %d %d %d %d %d %s\n", player->id, player->x,
            player->y, player->dir + 1, player->lvl, player->team);
        dprintf(gui_fd, "pin %d %d %d %d %d %d %d %d %d %d\n",
            player->id, player->x, player->y,
            player->inventory[FOOD],
            player->inventory[LINEMATE],
            player->inventory[DERAUMERE],
            player->inventory[SIBUR],
            player->inventory[MENDIANE],
            player->inventory[PHIRAS],
            player->inventory[THYSTAME]);
    }
}

static void send_teams_to_gui(int gui_fd, server_config_t *config)
{
    for (int i = 0; i < config->team_nb; i++) {
        dprintf(gui_fd, "tna %s\n", config->teams[i].name);
    }
}

void send_gui(server_t *serv, const char *format, ...)
{
    va_list args;

    if (serv->gui_fd == -1)
        return;
    va_start(args, format);
    vdprintf(serv->gui_fd, format, args);
    va_end(args);
}

static void send_frequency_to_gui(server_t *server)
{
    if (server->gui_fd == -1)
        return;
    dprintf(server->gui_fd, "sgt %d\n", server->config->freq);
}

void send_data_gui(server_t *server, int gui_fd, server_config_t *config)
{
    send_frequency_to_gui(server);
    send_map_size_to_gui(gui_fd, server->map);
    send_map_content_to_gui(gui_fd, server->map);
    send_players_to_gui(gui_fd, server);
    send_teams_to_gui(gui_fd, config);
}

static void send_tile_to_gui(server_t *server, int x, int y, tile_t *tile)
{
    for (int i = 1; i < NB_CONNECTION + 1; i++) {
        if (server->pfds[i].fd == FD_NULL ||
                server->clients[i].type != CLIENT_GUI)
            continue;
        dprintf(server->pfds[i].fd, "bct %d %d %d %d %d %d %d %d %d\n", x, y,
            tile->resources[FOOD],
            tile->resources[LINEMATE],
            tile->resources[DERAUMERE],
            tile->resources[SIBUR],
            tile->resources[MENDIANE],
            tile->resources[PHIRAS],
            tile->resources[THYSTAME]);
    }
}

void send_gui_resource_changes(server_t *server)
{
    map_t *map = server->map;
    tile_t *tile = NULL;

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            tile = &map->tiles[y][x];
            printf("Tile changed at (%d, %d): [%d %d %d %d %d %d %d]\n",
                x, y,
                tile->resources[FOOD],
                tile->resources[LINEMATE],
                tile->resources[DERAUMERE],
                tile->resources[SIBUR],
                tile->resources[MENDIANE],
                tile->resources[PHIRAS],
                tile->resources[THYSTAME]);
            send_tile_to_gui(server, x, y, tile);
            tile->changed = false;
        }
    }
}
