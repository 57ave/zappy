/*
** EPITECH PROJECT, 2025
** zappy gui class
** File description:
** gui.hpp
*/

#ifndef GUI_HPP_
#define GUI_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <queue>

#include "../include/pars_args.hpp"
#include "../include/Map.hpp"
#include "../include/Tile.hpp"

class gui {
    private:
        int sock;
        sockaddr_in serv_addr{};
        char buffer[1024];
        std::string recv_buffer;
        std::queue<std::string> message_queue;
        int windowSizeX;
        int windowSizeY;
        bool menu = true;
        sf::RenderWindow window;
        sf::Font font;
        float isoOffsetX = 0.0f;
        float isoOffsetY = 0.0f;
        Map map;
        Tile tile;

        void handle_server_data();
        void process_server_messages();
        void parse_msz(const std::string &message);
        void parse_bct(const std::string &message);
        void parse_server_data(const std::string &data);
        void drawMenu(sf::RenderWindow *window);
        void drawIsometricMap(sf::RenderWindow *window);
        void drawTopBar(sf::RenderWindow *window);
        void drawPlayers();
        void drawEggs();

    public:
        gui();
        ~gui();

        int connect_to_server(args_t *args);
        int run();
};

#endif // GUI_HPP_