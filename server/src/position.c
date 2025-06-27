/*
** EPITECH PROJECT, 2025
** commands.c
** File description:
** commands implementation
*/

#include "commands.h"
#include "server.h"

int calcul_pos_add(int player, int offset, int map)
{
    int a = (player + offset + map) % map;
    return a;
}

int calcul_pos_sub(int player, int offset, int map)
{
    int a = (player - offset + map) % map;
    return a;
}
