/*
** EPITECH PROJECT, 2025
** commands.h
** File description:
** commands
*/

#ifndef COMMANDS_H_
    #define COMMANDS_H_
    #include "server.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <math.h>
    #include <sys/time.h>
    #include <unistd.h>
    #define CMD_FORWARD_TIME 7
    #define CMD_RIGHT_TIME 7
    #define CMD_LEFT_TIME 7
    #define CMD_LOOK_TIME 7
    #define CMD_INVENTORY_TIME 1
    #define CMD_BROADCAST_TIME 7
    #define CMD_FORK_TIME 42
    #define CMD_EJECT_TIME 7
    #define CMD_TAKE_TIME 7
    #define CMD_SET_TIME 7
    #define CMD_INCANTATION_TIME 300

typedef struct {
    char *name;
    int time;
    void (*execute)(server_t *server, player_t *player, char *args);
} command_t;

typedef struct {
    int x;
    int y;
} position_t;

typedef struct {
    int level;
    int offset;
} look_params_t;

typedef struct {
    server_t *server;
    position_t pos;
} tile_context_t;

typedef struct {
    char *response;
    bool *first_tile;
} response_context_t;

typedef struct {
    server_t *server;
    player_t *player;
    response_context_t *resp_ctx;
} process_context_t;

typedef struct {
    int required_players;
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
} elevation_requirements_t;

void cmd_forward(server_t *server, player_t *player);
void cmd_right(server_t *serv, player_t *player);
void cmd_left(server_t *serv, player_t *player);
void cmd_look(server_t *server, player_t *player);
void cmd_inventory(server_t *server, player_t *player);
void cmd_broadcast(server_t *server, player_t *player, char *args);
void cmd_connect_nbr(server_t *server, player_t *player);
void cmd_fork(server_t *server, player_t *player);
void cmd_eject(server_t *server, player_t *player);
void cmd_take(server_t *server, player_t *player, char *args);
void cmd_set(server_t *server, player_t *player, char *args);
void cmd_incantation(server_t *server, player_t *player);

void execute_command(server_t *server, player_t *player, char *command);
void process_completed_actions(server_t *server);
resource_type_t get_resource_type(const char *name);
const char *get_resource_name(resource_type_t type);
int count_players_on_tile(server_t *server, int x, int y);
position_t calculate_look_coordinates(player_t *player, server_t *server,
    look_params_t params);
char *create_tile_content(tile_context_t *ctx);
bool build_tile_content(tile_context_t *tile_ctx, char *tile_str,
    size_t max_size);
void add_players_to_tile_content(int player_count, char *tile_content,
    bool *first_item);
void add_resources_to_tile_content(tile_t *tile, char *tile_content,
    bool *first_item);
void parse_command_args(char *command_copy, char **cmd_name, char **args);
void remove_action_from_queue(player_t *player);
void process_player_action(server_t *server, player_t *player);
int calculate_total_tiles(int vision_range);
int count_tile_elements(server_t *server, position_t pos);
int calculate_total_elements(server_t *server, player_t *player);
size_t calculate_response_size(server_t *server, player_t *player);
size_t calculate_element_size(const char *name, int value);
size_t calculate_tile_content_size(tile_context_t *ctx);
char *create_resource_info(int resource_index, int quantity);
bool add_single_resource(tile_t *tile, char *tile_content, bool *first_item,
    int r);
char *create_player_info(int player_count);
char *format_inventory_item(int resource_index, int quantity);
size_t calculate_item_size(int resource_index, int quantity);
size_t calculate_total_size(player_t *player);
void add_item_to_response(char *response, const char *item, bool first);
bool process_single_item(player_t *player, int i, char *response, bool *first);
position_t calculate_ejection_position(player_t *ejector, server_t *server);
bool eject_player_from_tile(server_t *server, player_t *ejector,
    player_t *target);
char *build_inventory_response(player_t *player);
bool process_single_tile(process_context_t *ctx, look_params_t params);
bool process_vision_level(process_context_t *ctx, int level);
char *build_look_response(server_t *server, player_t *player);
bool add_tile_to_response(response_context_t *ctx, const char *tile_content);
char *create_element_string(const char *name, int value);
bool add_element_to_response(response_context_t *ctx, const char *element);
bool add_tile_players(tile_context_t *tile_ctx, response_context_t *resp_ctx);
bool add_tile_resources(tile_context_t *tile_ctx,
    response_context_t *resp_ctx);
int calcul_pos_sub(int player, int offset, int map);
int calcul_pos_add(int player, int offset, int map);
void add_command_with_time(player_t *player, const char *command, int time);
char *create_command_copy(const char *original);
void execute_movement_and_info_commands(server_t *server, player_t *player,
    const char *cmd_name, const char *safe_args);
void handle_movement_commands(player_t *player, const char *cmd_name,
    const char *original_command);
void handle_graphic_client_registration(server_t *server, int i);
void handle_team_command(server_t *server, server_config_t *config,
    int client_index, const char *buffer);
void process_new_connections(server_t *server);
void remove_disconnected_clients(server_t *server);
void update_game_state(server_t *server);
void init_response_calculation(player_t *player, size_t *total_size,
    int *vision_range);
size_t add_separator_size(server_t *server, player_t *player,
    size_t base_size);
size_t calculate_all_levels_size(server_t *server, player_t *player,
    int vision_range);
size_t process_vision_level_size(server_t *server, player_t *player,
    int level);
void update_player_actions(server_t *server);
elevation_requirements_t get_elevation_requirements(int level);
int count_same_level_players(server_t *server, player_t *player);
bool check_tile_resources(server_t *server, player_t *player,
    elevation_requirements_t req);
bool validate_incantation_requirements(server_t *server, player_t *player,
    elevation_requirements_t req);
void elevate_all_participants(server_t *server, player_t *initiator);
void consume_incantation_resources(server_t *server, player_t *player,
    elevation_requirements_t req);
#endif /* !COMMANDS_H_ */
