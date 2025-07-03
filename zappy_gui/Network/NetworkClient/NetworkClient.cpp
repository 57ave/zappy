/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** NetworkClient
*/

#include "NetworkClient.hpp"

NetworkClient::NetworkClient() : _socket(-1), _running(false)
{
}

NetworkClient::NetworkClient(const std::string& ip, int port)
    : _socket(-1), _running(false)
{
    if (!connectToServer(ip, port)) {
        throw std::runtime_error("Failed to connect to server");
    }
}


NetworkClient::~NetworkClient()
{
    stopReceive();
    if (_socket != -1)
        close(_socket);
}

void NetworkClient::startReceive()
{
    _running = true;
    _networkThread = std::thread(&NetworkClient::networkLoop, this);
}

void NetworkClient::stopReceive()
{
    _running = false;
    if (_networkThread.joinable())
        _networkThread.join();
}

void NetworkClient::networkLoop()
{
    while (_running) {
        receiveMessage();
    }
}

bool NetworkClient::connectToServer(const std::string &ip, int port)
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        perror("socket");
        return false;
    }
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(port);
    if (ip == "localhost") {
        inet_pton(AF_INET, "127.0.0.1", &_serverAddr.sin_addr);
    } else {
        if (inet_pton(AF_INET, ip.c_str(), &_serverAddr.sin_addr) <= 0) {
            perror("inet_pton");
            close(_socket);
            return false;
        }
    }
    if (connect(_socket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0) {
        perror("connect");
        close(_socket);
        _socket = -1;
        return false;
    }
    fcntl(_socket, F_SETFL, O_NONBLOCK);
    return true;
}

void NetworkClient::sendMessage(const std::string &msg)
{
    ssize_t totalSent = 0;
    ssize_t toSend = msg.size();

    while (totalSent < toSend) {
        ssize_t sent = send(_socket, msg.c_str() + totalSent, toSend - totalSent, 0);
        if (sent <= 0) {
            perror("send");
            break;
        }
        totalSent += sent;
    }
}

void NetworkClient::receiveMessage()
{
    ssize_t bytesRead = recv(_socket, _buffer, sizeof(_buffer) - 1, 0);
    if (bytesRead == 0) {
        _running = false;
        return;
    }
    if (bytesRead < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("recv");
            _running = false;
        }
        return;
    }
    _buffer[bytesRead] = '\0';
    _recvBuffer += std::string(_buffer);
    size_t pos;
    while ((pos = _recvBuffer.find('\n')) != std::string::npos) {
        std::string message = _recvBuffer.substr(0, pos);
        _recvBuffer.erase(0, pos + 1);
        if (!message.empty()) {
            if (message == "WELCOME" && !_startMsg) {
                sendMessage("GRAPHIC\n");
                _startMsg = true;
            } else {
                std::lock_guard<std::mutex> lock(_queueMutex);
                _messageQueue.push(message);
            }
        }
    }
}

bool NetworkClient::tryPopMessage(std::string &msg)
{
    std::lock_guard<std::mutex> lock(_queueMutex);
    if (_messageQueue.empty())
        return false;

    msg = _messageQueue.front();
    _messageQueue.pop();
    return true;
}

