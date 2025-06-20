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
    if (zappy_gui.connect_to_server(&args) != 0) {
        std::cerr << "Erreur de connexion\n";
        return 84;
    }
    while (1) {
        zappy_gui.handle_server_data();
    }
    if (zappy_gui.run() != 0) {
        std::cerr << "Erreur lors de l'exécution du GUI\n";
        return 84;
    }
    return 0;
}
