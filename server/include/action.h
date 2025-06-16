
#ifndef ACTIONS_H_
#define ACTIONS_H_

#include "server.h"
#include "player.h"
#include "map.h"
#include "action.h"

int get_resource_index(const char *name);
team_t *get_team_by_name(server_config_t *config, const char *name);
void describe_tile(tile_t *tile, char *buffer, size_t size);
void look_direction(server_t *server, player_t *player, char *response, int dist);
void look_around(server_t *server, player_t *player);
void handle_movement(server_t *server, player_t *player);
void handle_turn(player_t *player, int right);
void handle_inventory(player_t *player);
void handle_take(server_t *server, player_t *player, const char *res);
void handle_set(server_t *server, player_t *player, const char *res);
void execute_action(server_t *server, player_t *player, action_t *action);

#endif /* !ACTIONS_H_ */
