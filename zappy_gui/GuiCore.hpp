/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GuiCore
*/

#ifndef GUICORE_HPP_
    #define GUICORE_HPP_
    #include <memory>
    #include "Network/INetwork.hpp"
    #include "Render/IRender.hpp"

class GuiCore {
    public:
        GuiCore(std::unique_ptr<INetwork> network, std::unique_ptr<IRender> render);
        ~GuiCore() = default;
        void run();
        void processNetworkMsg();
        void updateGameState();
    private:
        std::unique_ptr<INetwork> _network;
        std::unique_ptr<IRender> _render;
};

#endif /* !GUICORE_HPP_ */
