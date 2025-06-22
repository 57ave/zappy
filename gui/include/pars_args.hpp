/*
** EPITECH PROJECT, 2025
** zappy pars args gui
** File description:
** pars_args.hpp
*/

#ifndef PARS_ARGS_GUI_HPP_
#define PARS_ARGS_GUI_HPP_

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

typedef struct args_s {
    int port;
    std::string hostname;
} args_t;

bool check_args(int argc, char **argv);
void pars_args(int argc, char**argv, args_t *args);

#endif
