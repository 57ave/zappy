/*
** EPITECH PROJECT, 2025
** player.h
** File description:
** player
*/

#ifndef PLAYER_H_
    #define PLAYER_H_
    #include "map.h"

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
    char *team;
} player_t;

player_t *create_player(int id, int fd, const char *team, map_t *map);

#endif /* !PLAYER_H_ */
