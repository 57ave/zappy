/*
** EPITECH PROJECT, 2025
** zappy pars args gui
** File description:
** pars_args.cpp
*/

#include "../include/pars_args.hpp"

bool check_args(int argc, char **argv)
{
    if (argc < 5) {
        std::cerr << "Not enough arguments" << std::endl;
        return false;
    }
    if (argc > 5) {
        std::cerr << "Too many arguments" << std::endl;
        return false;
    }
    if (strcmp(argv[1], "-p") != 0 || strcmp(argv[3], "-h") != 0) {
        std::cerr << "Invalid arguments" << std::endl;
        return false;
    }
    return true;
}

void pars_args(int argc, char**argv, args_t *args)
{
    args->port = atoi(argv[2]);
    if (args->port <= 0) {
        std::cerr << "Invalid port number" << std::endl;
        exit(84);
    }
    args->hostname = argv[4];
    if (args->hostname.empty()) {
        std::cerr << "Hostname cannot be empty" << std::endl;
        exit(84);
    }
}