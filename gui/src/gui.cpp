/*
** EPITECH PROJECT, 2025
** zappy gui
** File description:
** gui.cpp
*/

#include "../include/gui.hpp"

gui::gui() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
}

gui::~gui() {
}

int gui::run() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Zappy");
    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        handle_server_data();
        parse_server_data(buffer);
        window.clear(sf::Color::White);
        window.display();
    }
    return 0;
}

int gui::connect_to_server(args_t *args) {
    serv_addr.sin_port = htons(args->port);
    inet_pton(AF_INET, args->hostname.c_str(), &serv_addr.sin_addr);
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        return 84;
    }
    const char* teamname = "GRAPHIC\n";
    send(sock, teamname, strlen(teamname), 0);
    return 0;
}

void gui::handle_server_data() {
    int valread = read(sock, buffer, sizeof(buffer) - 1);
    if (valread > 0) {
        buffer[valread] = '\0';
    } else if (valread < 0) {
        std::cerr << "Error reading from server" << std::endl;
    }
}

void gui::parse_server_data(const std::string &data) {
    std::cout << "Server data: " << data << std::endl;
}