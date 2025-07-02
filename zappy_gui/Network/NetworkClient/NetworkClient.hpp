/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** NetworkClient
*/

#ifndef NETWORKCLIENT_HPP_
    #define NETWORKCLIENT_HPP_
    #include "../INetwork.hpp"
    #include <string>
    #include <queue>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <iostream>
    #include <thread>
    #include <atomic>
    #include <chrono>
    #include <mutex>

class NetworkClient : public INetwork {
    public:
        NetworkClient();
        NetworkClient(const std::string &ip, int port);
        ~NetworkClient();
        bool connectToServer(const std::string &ip, int port) override;
        void receiveMessage() override;
        void sendMessage(const std::string &msg) override;
        void startReceive() override;
        void stopReceive() override;
        void networkLoop();
        bool tryPopMessage(std::string &msg) override;
    private:
        int _socket;
        sockaddr_in _serverAddr{};
        char _buffer[1024];
        std::string _recvBuffer;
        std::queue<std::string> _messageQueue;
        std::thread _networkThread;
        std::atomic<bool> _running;
        std::mutex _queueMutex;
        bool _startMsg = false;
};

#endif /* !NETWORKCLIENT_HPP_ */
