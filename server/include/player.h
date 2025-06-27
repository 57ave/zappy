/*
** EPITECH PROJECT, 2025
** player.h
** File description:
** player
*/

#ifndef PLAYER_H_
    #define PLAYER_H_
    #include "map.h"

typedef struct server_s server_t;

typedef struct {
    char *name;
    int max_players;
    int actual_players;
    int eggs_available;
} team_t;

typedef enum {
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
} direction_t;

typedef struct action_s {
    char *command;
    int remaining_ticks;
    struct action_s *next;
} action_t;

typedef struct player_s {
    int id;
    int fd;
    int x;
    int y;
    int lvl;
    direction_t direction;
    action_t *action_queue;
    int inventory[RESOURCE_COUNT];
    int life_remain;
    int food_tick;
    char *team;
} player_t;

// Fonctions de base
player_t *create_player(int id, int fd, const char *team, map_t *map);
void free_player(player_t *player);

// Gestion de la queue d'actions
void add_action_to_queue(player_t *player, const char *cmd, int time);
void remove_completed_action(player_t *player);
void clear_action_queue(player_t *player);
int get_queue_size(player_t *player);

// Mise à jour (utilise forward declaration)
void update_player_actions(server_t *server);

#endif /* !PLAYER_H_ */
