/*
** EPITECH PROJECT, 2025
** client.h
** File description:
** client
*/

#ifndef CLIENT_H_
    #define CLIENT_H_
    #define BUF_SIZE 1024

typedef struct {
    int fd;
    char read_buf[BUF_SIZE];
    int read_len;
} client_t;

#endif /* !CLIENT_H_ */
