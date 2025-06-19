/*
** EPITECH PROJECT, 2025
** commands.h
** File description:
** commands
*/

#ifndef COMMANDS_H_
    #define COMMANDS_H_
    #include "server.h"

// Temps d'exécution des commandes (en ticks)
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

// Structure pour les commandes
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

// Fonctions de commandes principales
void cmd_forward(server_t *server, player_t *player, char *args);
void cmd_right(server_t *server, player_t *player, char *args);
void cmd_left(server_t *server, player_t *player, char *args);
void cmd_look(server_t *server, player_t *player, char *args);
void cmd_inventory(server_t *server, player_t *player, char *args);
void cmd_broadcast(server_t *server, player_t *player, char *args);
void cmd_connect_nbr(server_t *server, player_t *player, char *args);
void cmd_fork(server_t *server, player_t *player, char *args);
void cmd_eject(server_t *server, player_t *player, char *args);
void cmd_take(server_t *server, player_t *player, char *args);
void cmd_set(server_t *server, player_t *player, char *args);
void cmd_incantation(server_t *server, player_t *player, char *args);

void execute_command(server_t *server, player_t *player, char *command);
void process_completed_actions(server_t *server);
resource_type_t get_resource_type(const char *name);
const char *get_resource_name(resource_type_t type);

#endif /* !COMMANDS_H_ */