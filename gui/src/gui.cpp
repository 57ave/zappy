/*
** EPITECH PROJECT, 2025
** zappy gui
** File description:
** gui.cpp
*/

#include "../include/gui.hpp"

gui::gui()
{
}

gui::~gui()
{
}

int gui::run_sfml()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Zappy");
    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::White);
        window.display();
    }
    return 0;
}