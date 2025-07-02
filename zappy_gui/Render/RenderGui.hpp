/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** RenderGui.hpp
*/

#ifndef RENDER_GUI_HPP
#define RENDER_GUI_HPP

#include "IRender.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <deque>
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/Player.hpp"
#include "Game/Egg.hpp"
#include "Game/GameState.hpp"

class Render : public IRender {
    public:
        Render();
        ~Render() override;

        void init(int width, int height) override;
        void render(const GameState &gameState) override;
        bool isOpen() const override;
        void handleEvents() override;
        void close() override;

    private:
        std::unique_ptr<sf::RenderWindow> _window;
        sf::Font _font;
        sf::Texture _playerTexture, _eggTexture, _resourcesTexture;
        int _width = 0, _height = 0;
        bool menu, endGame;
        float _isoOffsetX = 0.f, _isoOffsetY = 0.f;
        float _zoom = 1.f;

        void drawMenu();
        void drawEndGame(const GameState &gameState);
        void drawGame(const GameState &gameState);
        void drawMap(const GameState &gameState);
        void drawTopBar(const GameState &gameState);
        void drawEggs(const GameState &gameState);
        void drawPlayers(const GameState &gameState);
        void drawResources(const GameState &gameState);
        void drawPopMessages(const GameState &gameState);
};

#endif // RENDER_GUI_HPP