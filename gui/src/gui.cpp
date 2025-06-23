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
    serv_addr.sin_family = AF_INET;
    if (!font.loadFromFile("assets/font.ttf")) {
        std::cerr << "Erreur de chargement de la police" << std::endl;
        return;
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

void gui::parse_server_data(const std::string &data) {
    std::cout << data << std::endl;
}

int gui::run() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Zappy");
    window.setFramerateLimit(60);
    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        handle_server_data();
        process_server_messages();
        if (welcome_received == false)
            window.close();
        if (menu == true) {
            window.clear(sf::Color::Yellow);
            drawMenu(&window);
            window.display();
        } else {
            window.clear(sf::Color::Yellow);
            drawIsometricMap(&window, 10, 10);
            drawTopBar(&window);
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
    title.setFillColor(sf::Color::Black);
    title.setPosition(window->getSize().x / 2.0f - title.getGlobalBounds().width / 2.0f, 100);

    sf::Text subtitle("The game is about managing a world and its inhabitants.\n"
        "This world, Trantor is geographically made of zero-relief planes:\n"
        "no craters, no valleys, no mountains.\n"
        "The game board represents the entirety of this world s surface, like a world map.", font, 30);
    subtitle.setFillColor(sf::Color::Black);
    subtitle.setPosition(window->getSize().x / 2.0f - subtitle.getGlobalBounds().width / 2.0f, 230);

    sf::Text controls("Use Z, Q, S, D to move the map", font, 30);
    controls.setFillColor(sf::Color::Black);
    controls.setPosition(window->getSize().x / 2.0f - controls.getGlobalBounds().width / 2.0f, 500);

    sf::Text instructions("Press 'Enter' to switch to the map view", font, 30);
    instructions.setFillColor(sf::Color::Black);
    instructions.setPosition(window->getSize().x / 2.0f - instructions.getGlobalBounds().width / 2.0f, 550);

    window->draw(title);
    window->draw(subtitle);
    window->draw(controls);
    window->draw(instructions);
}

void gui::drawIsometricMap(sf::RenderWindow *window, int mapWidth, int mapHeight) {
    float moveSpeed = 20.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        isoOffsetY -= moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        isoOffsetY += moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        isoOffsetX -= moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        isoOffsetX += moveSpeed;
    float tileWidth = 64.0f;
    float tileHeight = 32.0f;
    float originX = window->getSize().x / 2.0f + isoOffsetX;
    float originY = 100.0f + isoOffsetY;

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            float isoX = (x - y) * (tileWidth / 2.0f);
            float isoY = (x + y) * (tileHeight / 2.0f);

            sf::ConvexShape diamond(4);
            diamond.setPoint(0, sf::Vector2f(0, tileHeight / 2.0f));
            diamond.setPoint(1, sf::Vector2f(tileWidth / 2.0f, 0));
            diamond.setPoint(2, sf::Vector2f(tileWidth, tileHeight / 2.0f));
            diamond.setPoint(3, sf::Vector2f(tileWidth / 2.0f, tileHeight));
            diamond.setFillColor(sf::Color(60, 180, 60));
            diamond.setOutlineColor(sf::Color(40, 120, 40));
            diamond.setOutlineThickness(1.0f);
            diamond.setPosition(originX + isoX - tileWidth / 2.0f, originY + isoY);
            window->draw(diamond);
        }
    }
}

void gui::drawTopBar(sf::RenderWindow *window) {
    sf::RectangleShape topBar(sf::Vector2f(window->getSize().x, 80));
    topBar.setFillColor(sf::Color::White);
    topBar.setOutlineColor(sf::Color::Black);
    topBar.setOutlineThickness(2.0f);
    window->draw(topBar);
    
    sf::Text title("Player status:", font, 30); 
    title.setFillColor(sf::Color::Black);
    title.setPosition(10, 15);
    window->draw(title);
}