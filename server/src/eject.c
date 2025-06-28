/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

position_t calculate_ejection_position(player_t *ejector, server_t *server)
{
    position_t pos = {ejector->x, ejector->y};

    switch (ejector->dir) {
        case UP:
            pos.y = (pos.y - 1 + server->map->height) % server->map->height;
            break;
        case RIGHT:
            pos.x = (pos.x + 1) % server->map->width;
            break;
        case DOWN:
            pos.y = (pos.y + 1) % server->map->height;
            break;
        case LEFT:
            pos.x = (pos.x - 1 + server->map->width) % server->map->width;
            break;
    }
    return pos;
}

bool eject_player_from_tile(server_t *server, player_t *ejector,
    player_t *target)
{
    position_t new_pos;

    if (target == ejector || target->x != ejector->x ||
        target->y != ejector->y) {
        return false;
    }
    new_pos = calculate_ejection_position(ejector, server);
    target->x = new_pos.x;
    target->y = new_pos.y;
    dprintf(target->fd, "eject: %d\n", (ejector->dir + 2) % 4);
    dprintf(server->gui_fd, "pex #%d\n", ejector->id);
    dprintf(server->gui_fd, "ppo #%d %d %d %d\n",
            target->id, target->x, target->y, target->dir);
    return true;
}

void cmd_eject(server_t *server, player_t *player)
{
    bool ejected_someone = false;

    for (int i = 0; i < server->player_nb; i++) {
        if (eject_player_from_tile(server, player, server->players[i])) {
            ejected_someone = true;
        }
    }
    dprintf(player->fd, ejected_someone ? "ok\n" : "ko\n");
}

static void send_gui_pgt(server_t *server, player_t *player,
    resource_type_t res)
{
    tile_t *tile = &server->map->tiles[player->y][player->x];

    if (server->gui_fd == -1)
        return;
    dprintf(server->gui_fd, "pgt #%d %d\n", player->id, res);
    dprintf(server->gui_fd, "pin #%d %d %d %d %d %d %d %d %d %d\n",
        player->id, player->x, player->y,
        player->inventory[FOOD], player->inventory[LINEMATE],
        player->inventory[DERAUMERE], player->inventory[SIBUR],
        player->inventory[MENDIANE], player->inventory[PHIRAS],
        player->inventory[THYSTAME]);
    dprintf(server->gui_fd, "bct %d %d %d %d %d %d %d %d %d\n",
        player->x, player->y,
        tile->resources[FOOD], tile->resources[LINEMATE],
        tile->resources[DERAUMERE], tile->resources[SIBUR],
        tile->resources[MENDIANE], tile->resources[PHIRAS],
        tile->resources[THYSTAME]);
}

void cmd_take(server_t *server, player_t *player, char *args)
{
    resource_type_t resource = get_resource_type(args);
    tile_t *tile = &server->map->tiles[player->y][player->x];

    if (resource == RESOURCE_INVALID) {
        dprintf(player->fd, "ko\n");
        return;
    }
    if (tile->resources[resource] > 0) {
        tile->resources[resource]--;
        player->inventory[resource]++;
        dprintf(player->fd, "ok\n");
        send_gui_pgt(server, player, resource);
    }
    dprintf(player->fd, "ko\n");
}

static void send_gui_pdr(server_t *server, player_t *player,
    resource_type_t res)
{
    tile_t *tile = &server->map->tiles[player->y][player->x];

    if (server->gui_fd == -1)
        return;
    dprintf(server->gui_fd, "pdr #%d %d\n", player->id, res);
    dprintf(server->gui_fd, "pin #%d %d %d %d %d %d %d %d %d %d\n",
        player->id, player->x, player->y,
        player->inventory[FOOD], player->inventory[LINEMATE],
        player->inventory[DERAUMERE], player->inventory[SIBUR],
        player->inventory[MENDIANE], player->inventory[PHIRAS],
        player->inventory[THYSTAME]);
    dprintf(server->gui_fd, "bct %d %d %d %d %d %d %d %d %d\n",
        player->x, player->y,
        tile->resources[FOOD], tile->resources[LINEMATE],
        tile->resources[DERAUMERE], tile->resources[SIBUR],
        tile->resources[MENDIANE], tile->resources[PHIRAS],
        tile->resources[THYSTAME]);
}

void cmd_set(server_t *server, player_t *player, char *args)
{
    resource_type_t resource = get_resource_type(args);

    if (resource == RESOURCE_INVALID) {
        dprintf(player->fd, "ko\n");
        return;
    }
    if (player->inventory[resource] > 0) {
        player->inventory[resource]--;
        server->map->tiles[player->y][player->x].resources[resource]++;
        dprintf(player->fd, "ok\n");
        send_gui_pdr(server, player, resource);
    } else {
        dprintf(player->fd, "ko\n");
    }
}
