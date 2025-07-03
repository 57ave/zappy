/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** DrawGui.cpp
*/

#include "RenderGui.hpp"

void Render::drawMenu() {
    _window->clear(sf::Color(150, 220, 255));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        menu = false;
    }
    sf::Text title("Zappy", _font, 50);
    title.setFillColor(sf::Color::White);
    title.setPosition(_window->getSize().x / 2.f - title.getGlobalBounds().width / 2.f, 100);

    sf::Text subtitle(
        "The game is about managing a world and its inhabitants.\n"
        "This world, Trantor is geographically made of zero-relief planes:\n"
        "no craters, no valleys, no mountains.\n"
        "The game board represents the entirety of this world s surface, like a world map.",
        _font, 30);
    subtitle.setFillColor(sf::Color::White);
    subtitle.setPosition(_window->getSize().x / 2.f - subtitle.getGlobalBounds().width / 2.f, 230);

    sf::Text controls(
        "Use Z, Q, S, D to move the map and + - to zoom\n"
        "Hold press left button mouse on team name to show all players team information\n\n\n"
        "Press 'Enter' to switch to the map view",
        _font, 30);
    controls.setFillColor(sf::Color::White);
    controls.setPosition(_window->getSize().x / 2.f - subtitle.getGlobalBounds().width / 2.f, 500);

    _window->draw(title);
    _window->draw(subtitle);
    _window->draw(controls);
}

void Render::drawEndGame(const GameState &gameState) {
    _window->clear(sf::Color(150, 220, 255));
    sf::Text endText("Game Over\n", _font, 50);
    endText.setString(endText.getString() + "Winner: " + gameState.winnerTeam);
    endText.setFillColor(sf::Color::White);
    endText.setPosition(_window->getSize().x / 2.f - endText.getGlobalBounds().width / 2.f,
                        _window->getSize().y / 2.f - endText.getGlobalBounds().height / 2.f - 50);
    _window->draw(endText);

    sf::Text controls("Press 'Ctrl' + 'Enter' to exit", _font, 30);
    controls.setFillColor(sf::Color::White);
    controls.setPosition(_window->getSize().x / 2.f - controls.getGlobalBounds().width / 2.f,
                         _window->getSize().y / 2.f + endText.getGlobalBounds().height / 2.f + 20);
    _window->draw(controls);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        _window->close();
    }
}

void Render::drawGame(const GameState &gameState) {
    _window->clear(sf::Color(150, 220, 255));
    drawMap(gameState);
    drawResources(gameState);
    drawEggs(gameState);
    drawPlayers(gameState);
    drawPopMessages(gameState);
    drawGlobalInfo(gameState);
    drawTopBar(gameState);
}

void Render::drawMap(const GameState &gameState) {
    float tileWidth = 64.0f * _zoom;
    float tileHeight = 64.0f * _zoom;

    float mapWidthPx = gameState.map.getWidth() * tileWidth;
    float mapHeightPx = gameState.map.getHeight() * tileHeight;
    float originX = (_window->getSize().x - mapWidthPx) / 2.f + _isoOffsetX;
    float originY = (_window->getSize().y - mapHeightPx) / 2.f + _isoOffsetY;

    for (int y = 0; y < gameState.map.getHeight(); ++y) {
        for (int x = 0; x < gameState.map.getWidth(); ++x) {
            float posX = originX + x * tileWidth;
            float posY = originY + y * tileHeight;

            sf::RectangleShape tile(sf::Vector2f(tileWidth, tileHeight));
            tile.setPosition(posX, posY);
            tile.setFillColor(sf::Color(60, 180, 60));
            tile.setOutlineColor(sf::Color(40, 120, 40));
            tile.setOutlineThickness(1.0f);
            _window->draw(tile);
        }
    }
}

void Render::drawTopBar(const GameState &gameState) {
    sf::RectangleShape topBar(sf::Vector2f(_window->getSize().x, 80));
    topBar.setFillColor(sf::Color::White);
    topBar.setOutlineColor(sf::Color::Black);
    topBar.setOutlineThickness(2.0f);
    _window->draw(topBar);

    sf::Text title("Teams:", _font, 30);
    title.setFillColor(sf::Color::Black);
    title.setPosition(10, 15);
    _window->draw(title);

    sf::Text timerText("", _font, 30);
    int minutes = gameState.timeUnit / 60;
    if (gameState.timeUnit <= 60)
        timerText.setString("Timer: " + std::to_string(gameState.timeUnit) + "s");
    else
        timerText.setString("Timer: " + std::to_string(minutes) + "m " + std::to_string(gameState.timeUnit % 60) + "s");
    timerText.setFillColor(sf::Color::Black);
    timerText.setPosition(_window->getSize().x - 250, 15);
    _window->draw(timerText);

    for (size_t i = 0; i < gameState.teams.size(); ++i) {
        sf::Text teamText(gameState.teams[i], _font, 30);
        teamText.setFillColor(sf::Color(((i+1) * 100) % 255, ((i+1) * 50) % 255, ((i+1) * 150) % 255));
        teamText.setPosition(110 * (i+1), 15);
        _window->draw(teamText);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
            if (teamText.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                sf::RectangleShape moreInfoBox(sf::Vector2f(350, 1000));
                moreInfoBox.setFillColor(sf::Color(200, 200, 200, 200));
                moreInfoBox.setPosition(20, 82);
                _window->draw(moreInfoBox);
                int playerIndex = 0;
                for (const auto& player : gameState.players) {
                    if (player.getTeam() == gameState.teams[i]) {
                        sf::Text playerInfo("Id: " + std::to_string(player.getId()) +
                            " Lvl: " + std::to_string(player.getLevel()) +
                            " Loc: x = " + std::to_string(player.getX()) +
                            ", y = " + std::to_string(player.getY()) + "\n", _font, 20);
                        const auto& inv = player.getInventory();
                        std::string invStr = "Inv: food(" + std::to_string(inv[0]) + ") linemate(" + std::to_string(inv[1]) +
                            ") deraumere(" + std::to_string(inv[2]) + ")\nsibur(" + std::to_string(inv[3]) +
                            ") mendiane(" + std::to_string(inv[4]) + ") phiras(" + std::to_string(inv[5]) +
                            ") thystame(" + std::to_string(inv[6]) + ")";
                        playerInfo.setFillColor(sf::Color::Black);
                        playerInfo.setPosition(25, 90 + 100 * playerIndex);
                        playerInfo.setString(playerInfo.getString() + invStr);
                        _window->draw(playerInfo);
                        playerIndex++;
                    }
                }
            }
        }
    }
}

void Render::drawEggs(const GameState &gameState) {
    float tileWidth = 64.0f * _zoom;
    float tileHeight = 64.0f * _zoom;
    float mapWidthPx = gameState.map.getWidth() * tileWidth;
    float mapHeightPx = gameState.map.getHeight() * tileHeight;
    float originX = (_window->getSize().x - mapWidthPx) / 2.0f + _isoOffsetX;
    float originY = (_window->getSize().y - mapHeightPx) / 2.0f + _isoOffsetY;

    float scale = tileWidth / 24.0f;
    for (const auto& egg : gameState.eggs) {
        sf::Sprite sprite(_eggTexture);
        sprite.setScale(scale, scale);

        float spriteWidth = 16.0f * scale;
        float spriteHeight = 16.0f * scale;
        float posX = originX + egg.getX() * tileWidth + (tileWidth - spriteWidth) / 2.0f;
        float posY = originY + egg.getY() * tileHeight + (tileHeight - spriteHeight) / 2.0f;

        sprite.setPosition(posX, posY);
        _window->draw(sprite);
    }
}

void Render::drawPlayers(const GameState &gameState) {
    float tileWidth = 64.0f * _zoom;
    float tileHeight = 64.0f * _zoom;
    float mapWidthPx = gameState.map.getWidth() * tileWidth;
    float mapHeightPx = gameState.map.getHeight() * tileHeight;
    float originX = (_window->getSize().x - mapWidthPx) / 2.0f + _isoOffsetX;
    float originY = (_window->getSize().y - mapHeightPx) / 2.0f + _isoOffsetY;

    for (const auto& player : gameState.players) {
        int x = player.getX();
        int y = player.getY();
        int orientation = player.getDirection();

        if (orientation == 0)
            continue;
        sf::Sprite sprite(_playerTexture);
        if (orientation == 1)
            sprite.setTextureRect(sf::IntRect(0 * 32, 12 * 32, 32, 32));
        else if (orientation == 2)
            sprite.setTextureRect(sf::IntRect(0 * 32, 10 * 32, 32, 32));
        else if (orientation == 3)
            sprite.setTextureRect(sf::IntRect(0 * 32, 6 * 32, 32, 32));
        else if (orientation == 4)
            sprite.setTextureRect(sf::IntRect(0 * 32, 8 * 32, 32, 32));
        else
            continue;
        sprite.setScale(tileWidth / 32.0f, tileHeight / 32.0f);
        float posX = originX + x * tileWidth;
        float posY = originY + y * tileHeight;
        sprite.setPosition(posX, posY);
        _window->draw(sprite);
    }
}

void Render::drawResources(const GameState &gameState) {
    sf::IntRect spriteRects[7] = {
        {5 * 16, 2 * 16, 16, 16}, //food
        {1 * 16, 1 * 16, 16, 16}, //linemate
        {2 * 16, 2 * 16, 16, 16}, //deraumere
        {4 * 16, 2 * 16, 16, 16}, //sibur
        {5 * 16, 0 * 16, 16, 16}, //mendiane
        {5 * 16, 1 * 16, 16, 16}, //phiras
        {6 * 16, 2 * 16, 16, 16}  //thystame
    };

    float offsets[7][2] = {
        {0.5f, 0.1f},
        {0.85f, 0.15f},
        {0.9f, 0.5f},
        {0.85f, 0.85f},
        {0.5f, 0.9f},
        {0.15f, 0.85f},
        {0.1f, 0.5f}
    };

    float tileWidth = 64.0f * _zoom;
    float tileHeight = 64.0f * _zoom;
    float mapWidthPx = gameState.map.getWidth() * tileWidth;
    float mapHeightPx = gameState.map.getHeight() * tileHeight;
    float originX = (_window->getSize().x - mapWidthPx) / 2.0f + _isoOffsetX;
    float originY = (_window->getSize().y - mapHeightPx) / 2.0f + _isoOffsetY;
    Map _cpy = gameState.map;

    for (int j = 0; j < gameState.map.getHeight(); j++) {
        for (int i = 0; i < gameState.map.getWidth(); i++) {
            const auto& resources = _cpy.at(i, j).getResources();
            if (resources.empty())
                continue;
            int resCount = std::min((int)resources.size(), 7);
            for (int r = 0; r < resCount; ++r) {
                if (resources[r] > 0) {
                    sf::Sprite sprite(_resourcesTexture);
                    sprite.setTextureRect(spriteRects[r]);
                    sprite.setScale(tileWidth / 64.0f, tileHeight / 64.0f);

                    float posX = originX + i * tileWidth + offsets[r][0] * tileWidth - 8 * _zoom;
                    float posY = originY + j * tileHeight + offsets[r][1] * tileHeight - 8 * _zoom;

                    sprite.setPosition(posX, posY);
                    _window->draw(sprite);
                }
            }
        }
    }
}

void Render::drawPopMessages(const GameState &gameState) {
    float padding = 10.f;
    float margin = 20.f;
    float messageSpacing = 8.f;
    float y = _window->getSize().y - margin;

    for (auto it = gameState._popMessages.rbegin(); it != gameState._popMessages.rend(); ++it) {
        sf::Text popMessage(*it, _font, 30);
        popMessage.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = popMessage.getLocalBounds();
        float boxWidth = textBounds.width + 2 * padding;
        float boxHeight = textBounds.height + 2 * padding;

        float boxX = _window->getSize().x - boxWidth - margin;
        y -= boxHeight;

        sf::RectangleShape popMessageBox(sf::Vector2f(boxWidth, boxHeight));
        popMessageBox.setFillColor(sf::Color(0, 0, 0, 180));
        popMessageBox.setPosition(boxX, y);

        popMessage.setPosition(boxX + padding, y + padding - textBounds.top);

        _window->draw(popMessageBox);
        _window->draw(popMessage);

        y -= messageSpacing;
    }
}

void Render::drawGlobalInfo(const GameState &gameState) {
    sf::RectangleShape infoBox(sf::Vector2f(300, 250));
    infoBox.setFillColor(sf::Color(200, 200, 200, 200));
    infoBox.setPosition(_window.get()->getSize().x - 320, 90);
    infoBox.setOutlineColor(sf::Color::Black);
    infoBox.setOutlineThickness(2.0f);
    _window->draw(infoBox);

    std::string info = "Map Size: " + std::to_string(gameState.map.getWidth()) + "x" + std::to_string(gameState.map.getHeight()) + "\n";
    info += "Fps: 60\n";
    Map _cpy = gameState.map;
    int food = 0, linemate = 0, deraumere = 0, sibur = 0, mendiane = 0, phiras = 0, thystame = 0;
    for (int j = 0; j < gameState.map.getHeight(); j++) {
        for (int i = 0; i < gameState.map.getWidth(); i++) {
            const auto& resources = _cpy.at(i, j).getResources();
            food += resources[0];
            linemate += resources[1];
            deraumere += resources[2];
            sibur += resources[3];
            mendiane += resources[4];
            phiras += resources[5];
            thystame += resources[6];
        }
    }
    info += "Food: " + std::to_string(food) + "\n";
    info += "Linemate: " + std::to_string(linemate) + "\n";
    info += "Deraumere: " + std::to_string(deraumere) + "\n";
    info += "Sibur: " + std::to_string(sibur) + "\n";
    info += "Mendiane: " + std::to_string(mendiane) + "\n";
    info += "Phiras: " + std::to_string(phiras) + "\n";
    info += "Thystame: " + std::to_string(thystame) + "\n";
    sf::Text globalInfo(info, _font, 20);
    globalInfo.setFillColor(sf::Color::White);
    globalInfo.setPosition(infoBox.getPosition().x + 10, infoBox.getPosition().y + 10);
    _window->draw(globalInfo);
}
