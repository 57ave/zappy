/*
** EPITECH PROJECT, 2025
** zappy gui main
** File description:
** main.cpp
*/

#include "include/pars_args.hpp"
#include "include/gui.hpp"

int main(int argc, char**argv) {
    args_t args;
    gui zappy_gui;

    if (check_args(argc, argv) == false) {
        std::cout << "USAGE: ./zappy_gui -p port -h machine" << std::endl;
        return 84;
    }
    pars_args(argc, argv, &args);
    zappy_gui.run_sfml();
    return 0;
}