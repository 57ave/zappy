/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GuiCore
*/

#ifndef GUICORE_HPP_
    #define GUICORE_HPP_
    #include <memory>
    #include "../Network/INetwork.hpp"
    #include "../Render/IRender.hpp"
    #include "../Network/NetworkClient/NetworkClient.hpp"
    #include "../Network/NetworkParser/NetworkParser.hpp"

class GuiCore {
    public:
        GuiCore(std::unique_ptr<INetwork> network, std::unique_ptr<IRender> render);
        ~GuiCore() = default;
        void run();
        void processNetworkMsg();
        void updateGameState();
        void startNetworkReceive();
        bool tryPopMessage(std::string &msg);
        static bool parseArgs(int argc, char **argv, int &port, std::string& hostname);
    private:
        std::unique_ptr<INetwork> _network;
        std::unique_ptr<IRender> _render;
        NetworkParser _parser;
        bool _running;
};

#endif /* !GUICORE_HPP_ */
