/*
** EPITECH PROJECT, 2025
** INetwork
** File description:
** INetwork
*/

#ifndef INETWORK_HPP_
    #define INETWORK_HPP_
    #include <string>
    #include <queue>

class INetwork {
    public:
        virtual ~INetwork() = default;
        virtual bool connectToServer(const std::string &ip, int port) = 0;
        virtual void sendMessage(const std::string &msg) = 0;
        virtual void receiveMessage() = 0;
        virtual std::queue<std::string> &getMessageQueue() = 0;
};

#endif /* !INETWORK_HPP_ */
