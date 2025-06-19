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

// Table des commandes
static command_t commands[] = {
    {"Forward", CMD_FORWARD_TIME, cmd_forward},
    {"Right", CMD_RIGHT_TIME, cmd_right},
    {"Left", CMD_LEFT_TIME, cmd_left},
    {"Look", CMD_LOOK_TIME, cmd_look},
    {"Inventory", CMD_INVENTORY_TIME, cmd_inventory},
    {"Broadcast", CMD_BROADCAST_TIME, cmd_broadcast},
    {"Connect_nbr", 0, cmd_connect_nbr},
    {"Fork", CMD_FORK_TIME, cmd_fork},
    {"Eject", CMD_EJECT_TIME, cmd_eject},
    {"Take", CMD_TAKE_TIME, cmd_take},
    {"Set", CMD_SET_TIME, cmd_set},
    {"Incantation", CMD_INCANTATION_TIME, cmd_incantation},
    {NULL, 0, NULL}
};

resource_type_t get_resource_type(const char *name)
{
    if (strcmp(name, "food") == 0) return FOOD;
    if (strcmp(name, "linemate") == 0) return LINEMATE;
    if (strcmp(name, "deraumere") == 0) return DERAUMERE;
    if (strcmp(name, "sibur") == 0) return SIBUR;
    if (strcmp(name, "mendiane") == 0) return MENDIANE;
    if (strcmp(name, "phiras") == 0) return PHIRAS;
    if (strcmp(name, "thystame") == 0) return THYSTAME;
    return -1;
}

const char *get_resource_name(resource_type_t type)
{
    switch (type) {
        case FOOD: return "food";
        case LINEMATE: return "linemate";
        case DERAUMERE: return "deraumere";
        case SIBUR: return "sibur";
        case MENDIANE: return "mendiane";
        case PHIRAS: return "phiras";
        case THYSTAME: return "thystame";
        default: return "unknown";
    }
}

void cmd_forward(server_t *server, player_t *player, char *args)
{
    int new_x = player->x;
    int new_y = player->y;

    switch (player->direction) {
        case UP:
            new_y = (player->y - 1 + server->map->height) % server->map->height;
            break;
        case RIGHT:
            new_x = (player->x + 1) % server->map->width;
            break;
        case DOWN:
            new_y = (player->y + 1) % server->map->height;
            break;
        case LEFT:
            new_x = (player->x - 1 + server->map->width) % server->map->width;
            break;
    }
    player->x = new_x;
    player->y = new_y;
    dprintf(player->fd, "ok\n");
}

void cmd_right(server_t *server, player_t *player, char *args)
{
    player->direction = (player->direction + 1) % 4;
    dprintf(player->fd, "ok\n");
}

void cmd_left(server_t *server, player_t *player, char *args)
{
    player->direction = (player->direction - 1 + 4) % 4;
    dprintf(player->fd, "ok\n");
}

static position_t calculate_look_coordinates(player_t *player, server_t *server,
    look_params_t params)
{
    position_t pos;

    switch (player->direction) {
        case UP:
            pos.x = (player->x + params.offset + server->map->width) % server->map->width;
            pos.y = (player->y - params.level + server->map->height) % server->map->height;
            break;
        case RIGHT:
            pos.x = (player->x + params.level) % server->map->width;
            pos.y = (player->y + params.offset + server->map->height) % server->map->height;
            break;
        case DOWN:
            pos.x = (player->x - params.offset + server->map->width) % server->map->width;
            pos.y = (player->y + params.level) % server->map->height;
            break;
        case LEFT:
            pos.x = (player->x - params.level + server->map->width) % server->map->width;
            pos.y = (player->y - params.offset + server->map->height) % server->map->height;
            break;
    }
    return pos;
}

static void add_resources_to_tile_content(tile_t *tile, char *tile_content,
    bool *first_item)
{
    for (int r = 0; r < RESOURCE_COUNT; r++) {
        if (tile->resources[r] > 0) {
            if (!*first_item) strcat(tile_content, " ");
            char resource_info[64];
            snprintf(resource_info, sizeof(resource_info), "%s %d", 
                    get_resource_name(r), tile->resources[r]);
            strcat(tile_content, resource_info);
            *first_item = false;
        }
    }
}

static int count_players_on_tile(server_t *server, int x, int y)
{
    int player_count = 0;

    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i]->x == x && server->players[i]->y == y) {
            player_count++;
        }
    }
    return player_count;
}

static void add_players_to_tile_content(int player_count, char *tile_content,
    bool *first_item)
{
    if (player_count > 0) {
        if (!*first_item) strcat(tile_content, " ");
        char player_info[32];
        snprintf(player_info, sizeof(player_info), "player %d", player_count);
        strcat(tile_content, player_info);
    }
}

static void build_tile_content(server_t *server, position_t pos,
    char *tile_content)
{
    tile_t *tile = &server->map->tiles[pos.y][pos.x];
    bool first_item = true;
    int player_count = count_players_on_tile(server, pos.x, pos.y);

    strcpy(tile_content, "");
    add_resources_to_tile_content(tile, tile_content, &first_item);
    add_players_to_tile_content(player_count, tile_content, &first_item);
}

void cmd_look(server_t *server, player_t *player, char *args)
{
    char response[4096] = "[";
    char tile_content[256];
    int vision_range = player->lvl;
    bool first_tile = true;

    for (int level = 0; level <= vision_range; level++) {
        for (int offset = -level; offset <= level; offset++) {
            look_params_t params = {level, offset};
            position_t pos = calculate_look_coordinates(player, server, params);
            build_tile_content(server, pos, tile_content);
            if (!first_tile) strcat(response, ",");
            strcat(response, tile_content);
            first_tile = false;
        }
    }
    strcat(response, "]\n");
    dprintf(player->fd, "%s", response);
}

void cmd_inventory(server_t *server, player_t *player, char *args)
{
    char response[1024] = "[";
    bool first = true;

    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (player->inventory[i] > 0) {
            if (!first) strcat(response, ", ");
            char item[128];
            snprintf(item, sizeof(item), "%s %d", get_resource_name(i), player->inventory[i]);
            strcat(response, item);
            first = false;
        }
    }
    strcat(response, "]\n");
    dprintf(player->fd, "%s", response);
}

void cmd_broadcast(server_t *server, player_t *player, char *args)
{
    // Diffuser le message à tous les joueurs
    for (int i = 0; i < server->player_nb; i++) {
        if (server->players[i] != player) {
            // Calculer la direction du message (simplifié)
            int direction = 0;
            dprintf(server->players[i]->fd, "message %d,%s\n", direction, args);
        }
    }
    dprintf(player->fd, "ok\n");
}

void cmd_connect_nbr(server_t *server, player_t *player, char *args)
{
    // Trouver l'équipe du joueur
    team_t *team = find_team(player->team, server->config);
    if (team) {
        int available_slots = team->max_players - team->actual_players;
        dprintf(player->fd, "%d\n", available_slots);
    } else {
        dprintf(player->fd, "0\n");
    }
}

void cmd_fork(server_t *server, player_t *player, char *args)
{
    // Créer un œuf pour l'équipe
    team_t *team = find_team(player->team, server->config);
    if (team) {
        team->eggs_available++;
        dprintf(player->fd, "ok\n");
    } else {
        dprintf(player->fd, "ko\n");
    }
}

static position_t calculate_ejection_position(player_t *ejector, server_t *server)
{
    position_t pos = {ejector->x, ejector->y};
    
    switch (ejector->direction) {
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

static bool eject_player_from_tile(server_t *server, player_t *ejector,
    player_t *target)
{
    position_t new_pos;

    if (target == ejector || target->x != ejector->x || target->y != ejector->y) {
        return false;
    }
    new_pos = calculate_ejection_position(ejector, server);
    target->x = new_pos.x;
    target->y = new_pos.y;
    dprintf(target->fd, "eject: %d\n", (ejector->direction + 2) % 4);
    return true;
}

void cmd_eject(server_t *server, player_t *player, char *args)
{
    bool ejected_someone = false;
    
    for (int i = 0; i < server->player_nb; i++) {
        if (eject_player_from_tile(server, player, server->players[i])) {
            ejected_someone = true;
        }
    }
    dprintf(player->fd, ejected_someone ? "ok\n" : "ko\n");
}

void cmd_take(server_t *server, player_t *player, char *args)
{
    resource_type_t resource = get_resource_type(args);
    
    if (resource == -1) {
        dprintf(player->fd, "ko\n");
        return;
    }
    
    tile_t *tile = &server->map->tiles[player->y][player->x];
    
    if (tile->resources[resource] > 0) {
        tile->resources[resource]--;
        player->inventory[resource]++;
        dprintf(player->fd, "ok\n");
    } else {
        dprintf(player->fd, "ko\n");
    }
}

void cmd_set(server_t *server, player_t *player, char *args)
{
    resource_type_t resource = get_resource_type(args);
    
    if (resource == -1) {
        dprintf(player->fd, "ko\n");
        return;
    }
    
    if (player->inventory[resource] > 0) {
        player->inventory[resource]--;
        server->map->tiles[player->y][player->x].resources[resource]++;
        dprintf(player->fd, "ok\n");
    } else {
        dprintf(player->fd, "ko\n");
    }
}

void cmd_incantation(server_t *server, player_t *player, char *args)
{
    // Ici, on devrait vérifier les ressources nécessaires et le nombre de joueurs
    // Pour l'instant, on assume que c'est possible
    player->lvl++;
    dprintf(player->fd, "Elevation underway\nCurrent level: %d\n", player->lvl);
}

void execute_command(server_t *server, player_t *player, char *command)
{
    char *cmd_name = strtok(command, " \n");
    char *args = strtok(NULL, "\n");
    
    if (!cmd_name) {
        dprintf(player->fd, "ko\n");
        return;
    }
    
    // Chercher la commande dans la table
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(cmd_name, commands[i].name) == 0) {
            if (commands[i].time > 0) {
                // Ajouter à la queue d'actions
                add_action_to_queue(player, command, commands[i].time);
            } else {
                // Exécuter immédiatement
                commands[i].execute(server, player, args ? args : "");
            }
            return;
        }
    }
    
    // Commande inconnue
    dprintf(player->fd, "ko\n");
}

void process_completed_actions(server_t *server)
{
    for (int i = 0; i < server->player_nb; i++) {
        player_t *player = server->players[i];
        action_t *action = player->action_queue;
        
        if (action && action->remaining_ticks <= 0) {
            // Exécuter l'action
            char *cmd_name = strtok(action->command, " ");
            char *args = strtok(NULL, "");
            
            for (int j = 0; commands[j].name != NULL; j++) {
                if (strcmp(cmd_name, commands[j].name) == 0) {
                    commands[j].execute(server, player, args ? args : "");
                    break;
                }
            }
            
            // Retirer l'action de la queue
            player->action_queue = action->next;
            free(action->command);
            free(action);
        }
    }
}