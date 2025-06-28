/*
** EPITECH PROJECT, 2025
** zappy gui
** File description:
** gui.cpp
*/

#include "../include/gui.hpp"

static bool welcome_received = false;

gui::gui() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Erreur lors de la création de la socket" << std::endl;
    }
    menu = true;
    endGame = false;
    serv_addr.sin_family = AF_INET;
    if (!font.loadFromFile("assets/font.ttf")) {
        std::cerr << "Erreur de chargement de la police" << std::endl;
        return;
    }
    if (!music.openFromFile("assets/music.ogg")) {
        std::cerr << "Erreur de chargement de la musique" << std::endl;
    } else {
        music.setLoop(true);
        music.play();
        music.setVolume(50);
    }
}

gui::~gui() {
    if (sock >= 0)
        close(sock);
    menu = false;

}

int gui::connect_to_server(args_t *args) {
    serv_addr.sin_port = htons(args->port);
    inet_pton(AF_INET, args->hostname.c_str(), &serv_addr.sin_addr);
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connexion au serveur échouée" << std::endl;
        return 84;
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);
    const char* teamname = "GRAPHIC\n";
    if (send(sock, teamname, strlen(teamname), 0) < 0) {
        std::cerr << "Erreur lors de l'envoi du nom d'équipe" << std::endl;
        return 84;
    }
    return 0;
}

void gui::handle_server_data() {
    ssize_t bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        recv_buffer += buffer;
        size_t pos;
        while ((pos = recv_buffer.find('\n')) != std::string::npos) {
            std::string message = recv_buffer.substr(0, pos);
            message_queue.push(message);
            recv_buffer.erase(0, pos + 1);
        }
    }
}

void gui::process_server_messages()
{
    while (!message_queue.empty()) {
        std::string message = message_queue.front();
        message_queue.pop();
        if (!welcome_received) {
            if (message == "WELCOME") {
                welcome_received = true;
            }
        }
        parse_server_data(message.c_str());
    }
}

int gui::run() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Zappy");
    window.setFramerateLimit(60);
    sf::Event event;
    music.play();
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        handle_server_data();
        process_server_messages();
        if (welcome_received == false)
            window.close();
        if (gameClock.getElapsedTime().asSeconds() >= 1.0f) {
            timeGame++;
            gameClock.restart();
        }
        if (menu == true) {
            window.clear(sf::Color(150, 220, 255));
            drawMenu(&window);
            window.display();
        } else if (endGame == true) {
            drawEndGame(&window);
            window.display();
        } else {
            drawGame(&window);
            window.display();
        }
    }
    return 0;
}

void gui::drawMenu(sf::RenderWindow *window) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        menu = false;
    }
    sf::Text title("Zappy", font, 50);
    title.setFillColor(sf::Color::White);
    title.setPosition(window->getSize().x / 2.0f - title.getGlobalBounds().width / 2.0f, 100);

    sf::Text subtitle("The game is about managing a world and its inhabitants.\n"
        "This world, Trantor is geographically made of zero-relief planes:\n"
        "no craters, no valleys, no mountains.\n"
        "The game board represents the entirety of this world s surface, like a world map.", font, 30);
    subtitle.setFillColor(sf::Color::White);
    subtitle.setPosition(window->getSize().x / 2.0f - subtitle.getGlobalBounds().width / 2.0f, 230);

    sf::Text controls("Use Z, Q, S, D to move the map and + - to zoom\n", font, 30);
    controls.setFillColor(sf::Color::White);
    controls.setPosition(window->getSize().x / 2.0f - subtitle.getGlobalBounds().width / 2.0f, 500);
    controls.setString(controls.getString() + "Hold press left button mouse on team name to show all players team information\n\n\n" +
        "Press 'Enter' to switch to the map view");
    
    window->draw(title);
    window->draw(subtitle);
    window->draw(controls);
}

void gui::drawEndGame(sf::RenderWindow *window) {
    window->clear(sf::Color(150, 220, 255));
    sf::Text endText("Game Over\n", font, 50);
    endText.setString(endText.getString() + "Winner: " + winnerTeam);
    endText.setFillColor(sf::Color::White);
    endText.setPosition(window->getSize().x / 2.0f - endText.getGlobalBounds().width / 2.0f, window->getSize().y / 2.0f - endText.getGlobalBounds().height / 2.0f - 50);
    window->draw(endText);
    sf::Text controls("Press 'Ctrl' + 'Enter' to exit", font, 30);
    controls.setFillColor(sf::Color::White);
    controls.setPosition(window->getSize().x / 2.0f - controls.getGlobalBounds().width / 2.0f, window->getSize().y / 2.0f + endText.getGlobalBounds().height / 2.0f + 20);
    window->draw(controls);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        window->close();
    }
}

void gui::drawGame(sf::RenderWindow *window) {
    window->clear(sf::Color(150, 220, 255));
    drawMap(window);
    drawResources(window);
    drawEggs(window);
    drawPlayers(window);
    drawPopMessages(window);
    drawTopBar(window);
}

void gui::drawMap(sf::RenderWindow *window) {
    float moveSpeed = 20.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        isoOffsetY -= moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        isoOffsetY += moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        isoOffsetX -= moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        isoOffsetX += moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) {
        zoom += 0.05;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) {
        if (zoom > 0.2f)
            zoom -= 0.05;
    }
    float tileWidth = 64.0f * zoom;
    float tileHeight = 64.0f * zoom;

    float mapWidthPx = map.getWidth() * tileWidth;
    float mapHeightPx = map.getHeight() * tileHeight;
    float originX = (window->getSize().x - mapWidthPx) / 2.0f + isoOffsetX;
    float originY = (window->getSize().y - mapHeightPx) / 2.0f + isoOffsetY;

    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            float posX = originX + x * tileWidth;
            float posY = originY + y * tileHeight;

            sf::RectangleShape tile(sf::Vector2f(tileWidth, tileHeight));
            tile.setPosition(posX, posY);
            tile.setFillColor(sf::Color(60, 180, 60));
            tile.setOutlineColor(sf::Color(40, 120, 40));
            tile.setOutlineThickness(1.0f);
            window->draw(tile);
        }
    }
}

void gui::drawTopBar(sf::RenderWindow *window) {
    sf::RectangleShape topBar(sf::Vector2f(window->getSize().x, 80));
    topBar.setFillColor(sf::Color::White);
    topBar.setOutlineColor(sf::Color::Black);
    topBar.setOutlineThickness(2.0f);
    window->draw(topBar);
    
    sf::Text title("Teams:", font, 30); 
    title.setFillColor(sf::Color::Black);
    title.setPosition(10, 15);
    window->draw(title);

    sf::Text timerText(" ", font, 30);
    int minutes = timeGame / 60;
    if (timeGame <= 60) {
        timerText.setString("Timer: " + std::to_string(timeGame) + "s");
    } else {
        timerText.setString("Timer: " + std::to_string(minutes) + "m " + std::to_string(timeGame % 60) + "s");
    }
    timerText.setFillColor(sf::Color::Black);
    timerText.setPosition(window->getSize().x - 250, 15);
    window->draw(timerText);

    if (teams.empty()) {
        return;
    }
    for (size_t i = 0; i < teams.size(); ++i) {
        sf::Text teamText(teams[i], font, 30);
        teamText.setFillColor(sf::Color(((i+1) * 100) % 255, ((i+1) * 50) % 255, ((i+1) * 150) % 255));
        teamText.setPosition(110 * (i+1), 15);
        window->draw(teamText);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            if (teamText.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                sf::RectangleShape moreInfoBox(sf::Vector2f(350, 1000));
                moreInfoBox.setFillColor(sf::Color(200, 200, 200, 200));
                moreInfoBox.setPosition(20, 82);
                window->draw(moreInfoBox);
                int playerIndex = 0;
                for (const auto& player : players) {
                    if (player.getTeam() == teams[i]) {
                        sf::Text playerInfo("Id: " + std::to_string(player.getId()) +
                            " Lvl: " + std::to_string(player.getLevel()) +
                            " Loc: x = " + std::to_string(player.getX()) +
                            ", y = " + std::to_string(player.getY()) + "\n" , font, 20);
                        const auto& inv = player.getInventory();
                        std::string invStr = "Inv: food(" + std::to_string(inv[0]) + ") linemate(" + std::to_string(inv[1]) +
                            ") deraumere(" + std::to_string(inv[2]) + ")\nsibur(" + std::to_string(inv[3]) +
                            ") mendiane(" + std::to_string(inv[4]) + ") phiras(" + std::to_string(inv[5]) +
                            ") thystame(" + std::to_string(inv[6]) + ")";
                        playerInfo.setFillColor(sf::Color::Black);
                        playerInfo.setPosition(25, 90 + 100 * playerIndex);
                        playerInfo.setString(playerInfo.getString() + invStr);
                        window->draw(playerInfo);
                        playerIndex++;
                    }
                }
            }
        }
    }
}

void gui::drawEggs(sf::RenderWindow *window) {
    static sf::Texture eggTexture;
    if (!eggTexture.loadFromFile("assets/egg.png")) {
        std::cerr << "Erreur de chargement du sprite oeuf" << std::endl;
        return;
    }

    float tileWidth = 64.0f * zoom;
    float tileHeight = 64.0f * zoom;
    float mapWidthPx = map.getWidth() * tileWidth;
    float mapHeightPx = map.getHeight() * tileHeight;
    float originX = (window->getSize().x - mapWidthPx) / 2.0f + isoOffsetX;
    float originY = (window->getSize().y - mapHeightPx) / 2.0f + isoOffsetY;

    float scale = tileWidth / 24.0f;
    for (const auto& egg : eggs) {
        sf::Sprite sprite(eggTexture);
        sprite.setScale(scale, scale);

        float spriteWidth = 16.0f * scale;
        float spriteHeight = 16.0f * scale;
        float posX = originX + egg.getX() * tileWidth + (tileWidth - spriteWidth) / 2.0f;
        float posY = originY + egg.getY() * tileHeight + (tileHeight - spriteHeight) / 2.0f;

        sprite.setPosition(posX, posY);
        window->draw(sprite);
    }
}

void gui::drawPlayers(sf::RenderWindow *window) {
    static sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("assets/player.png")) {
        std::cerr << "Erreur de chargement du sprite joueur" << std::endl;
        return;
    }

    float tileWidth = 64.0f * zoom;
    float tileHeight = 64.0f * zoom;
    float mapWidthPx = map.getWidth() * tileWidth;
    float mapHeightPx = map.getHeight() * tileHeight;
    float originX = (window->getSize().x - mapWidthPx) / 2.0f + isoOffsetX;
    float originY = (window->getSize().y - mapHeightPx) / 2.0f + isoOffsetY;

    for (auto& player : players) {
        int x = player.getX();
        int y = player.getY();
        int orientation = player.getDirection();

        if (orientation == 0) {
            continue;
        }
        sf::Sprite sprite(playerTexture);
        if (orientation == 1) {
            sprite.setTextureRect(sf::IntRect(0 * 32, 12 * 32, 32, 32));
        } else if (orientation == 2) {
            sprite.setTextureRect(sf::IntRect(0 * 32, 10 * 32, 32, 32));
        } else if (orientation == 3) {
            sprite.setTextureRect(sf::IntRect(0 * 32, 6 * 32, 32, 32));
        } else if (orientation == 4) {
            sprite.setTextureRect(sf::IntRect(0 * 32, 8 * 32, 32, 32));
        } else
            return;
        sprite.setScale(tileWidth / 32.0f, tileHeight / 32.0f);
        float posX = originX + x * tileWidth;
        float posY = originY + y * tileHeight;
        sprite.setPosition(posX, posY);
        window->draw(sprite);
    }
}

void gui::drawResources(sf::RenderWindow *window) {
    static sf::Texture ResourcesTexture;
    static bool textureLoaded = false;
    if (!textureLoaded) {
        if (!ResourcesTexture.loadFromFile("assets/materials.png")) {
            std::cerr << "Erreur de chargement des ressources" << std::endl;
            return;
        }
        textureLoaded = true;
    }
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

    float tileWidth = 64.0f * zoom;
    float tileHeight = 64.0f * zoom;
    float mapWidthPx = map.getWidth() * tileWidth;
    float mapHeightPx = map.getHeight() * tileHeight;
    float originX = (window->getSize().x - mapWidthPx) / 2.0f + isoOffsetX;
    float originY = (window->getSize().y - mapHeightPx) / 2.0f + isoOffsetY;

    for (int j = 0; j < map.getHeight(); j++) {
        for (int i = 0; i < map.getWidth(); i++) {
            Tile &tile = map.at(i, j);
            const auto& resources = tile.getResources();
            if (resources.empty())
                continue;
            int resCount = std::min((int)resources.size(), 7);
            for (int r = 0; r < resCount; ++r) {
                if (resources[r] > 0) {
                    sf::Sprite sprite(ResourcesTexture);
                    sprite.setTextureRect(spriteRects[r]);
                    sprite.setScale(tileWidth / 64.0f, tileHeight / 64.0f);

                    float posX = originX + i * tileWidth + offsets[r][0] * tileWidth - 8 * zoom;
                    float posY = originY + j * tileHeight + offsets[r][1] * tileHeight - 8 * zoom;

                    sprite.setPosition(posX, posY);
                    window->draw(sprite);
                }
            }
        }
    }
}

void gui::addPopMessage(const std::string& msg) {
    const size_t maxMessages = 10;
    if (popMessages.size() >= maxMessages) {
        popMessages.pop_front();
    }
    popMessages.push_back({msg});
}

void gui::drawPopMessages(sf::RenderWindow *window) {
    float padding = 10.f;
    float margin = 20.f;
    float messageSpacing = 8.f;
    float y = window->getSize().y - margin;

    for (auto it = popMessages.rbegin(); it != popMessages.rend(); ++it) {
        sf::Text popMessage(it->text, font, 30);
        popMessage.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = popMessage.getLocalBounds();
        float boxWidth = textBounds.width + 2 * padding;
        float boxHeight = textBounds.height + 2 * padding;

        float boxX = window->getSize().x - boxWidth - margin;
        y -= boxHeight;

        sf::RectangleShape popMessageBox(sf::Vector2f(boxWidth, boxHeight));
        popMessageBox.setFillColor(sf::Color(0, 0, 0, 180));
        popMessageBox.setPosition(boxX, y);

        popMessage.setPosition(boxX + padding, y + padding - textBounds.top);

        window->draw(popMessageBox);
        window->draw(popMessage);

        y -= messageSpacing;
    }
}