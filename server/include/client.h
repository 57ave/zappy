/*
** EPITECH PROJECT, 2025
** client.h
** File description:
** client
*/

#ifndef CLIENT_H_
    #define CLIENT_H_
    #define BUF_SIZE 1024
    #include "player.h"

typedef enum {
    CLIENT_IA,
    CLIENT_GUI
} client_type_t;

typedef struct {
    int fd;
    char read_buf[BUF_SIZE];
    int read_len;
    client_type_t type;
    player_t *player;
} client_t;

#endif /* !CLIENT_H_ */
