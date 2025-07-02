/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** RenderGui.cpp
*/

#include "RenderGui.hpp"

Render::Render() = default;

Render::~Render() {
    close();
}

void Render::init(int width, int height) {
    _width = width;
    _height = height;
    menu = true;
    endGame = false;
    _window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Zappy GUI");
    _font.loadFromFile("Render/assets/font.ttf");
    _playerTexture.loadFromFile("Render/assets/player.png");
    _eggTexture.loadFromFile("Render/assets/egg.png");
    _resourcesTexture.loadFromFile("Render/assets/materials.png");
}

void Render::render(const GameState &gameState) {
    if (!_window || !_window->isOpen())
        return;
    _window->clear(sf::Color::Black);

    if (menu) {
        drawMenu();
    } else if (endGame) {
        drawEndGame(gameState);
    } else {
        drawGame(gameState);
    }
    _window->display();
}

bool Render::isOpen() const {
    return _window && _window->isOpen();
}

void Render::handleEvents() {
    if (!_window)
        return;
    sf::Event event;
    while (_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            _window->close();
        float moveSpeed = 20.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            _isoOffsetY -= moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            _isoOffsetY += moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            _isoOffsetX -= moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            _isoOffsetX += moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) {
            _zoom += 0.05;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) {
            if (_zoom > 0.2f)
                _zoom -= 0.05;
        }
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta > 0) {
                _zoom += 0.05f;
            } else if (event.mouseWheelScroll.delta < 0 && _zoom > 0.2f) {
                _zoom -= 0.05f;
            }
        }
    }
}

void Render::close() {
    if (_window && _window->isOpen())
        _window->close();
}
