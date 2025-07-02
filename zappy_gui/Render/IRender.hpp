/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** IRender
*/

#ifndef IRENDER_HPP_
#define IRENDER_HPP_

class IRender {
    public:
        virtual ~IRender() = default;
        virtual void init(int width, int height) = 0;
        virtual void render() = 0;
        virtual bool isOpen() const = 0;
        virtual void handleEvents() = 0;
        virtual void close() = 0;
};

#endif /* !IRENDER_HPP_ */
