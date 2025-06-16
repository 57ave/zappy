
#include "server.h"
#include "actions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handle_client_message(server_t *server, int i, const char *buffer, server_config_t *config)
{
    if (server->clients[i].type == CLIENT_GUI) {
        write(server->pfds[i].fd, "WELCOME\n", 8);
    } else if (server->clients[i].type == CLIENT_IA &&
               server->clients[i].player != NULL) {
        player_t *player = server->clients[i].player;
        int freq = config->freq;

        if (strncmp(buffer, "Forward", 7) == 0)
            add_action_to_queue(player, "Forward", 7 / freq);
        else if (strncmp(buffer, "Right", 5) == 0)
            add_action_to_queue(player, "Right", 7 / freq);
        else if (strncmp(buffer, "Left", 4) == 0)
            add_action_to_queue(player, "Left", 7 / freq);
        else if (strncmp(buffer, "Look", 4) == 0)
            add_action_to_queue(player, "Look", 7 / freq);
        else if (strncmp(buffer, "Inventory", 9) == 0)
            add_action_to_queue(player, "Inventory", 1 / freq);
        else if (strncmp(buffer, "Take ", 5) == 0)
            add_action_to_queue(player, buffer, 7 / freq);
        else if (strncmp(buffer, "Set ", 4) == 0)
            add_action_to_queue(player, buffer, 7 / freq);
        else if (strncmp(buffer, "Connect_nbr", 11) == 0)
            add_action_to_queue(player, "Connect_nbr", 0);
        else if (strncmp(buffer, "Fork", 4) == 0)
            add_action_to_queue(player, "Fork", 42 / freq);
        else if (strncmp(buffer, "Eject", 5) == 0)
            add_action_to_queue(player, "Eject", 7 / freq);
        else
            write(server->pfds[i].fd, "ko\n", 3);
    } else {
        handle_team_command(server, config, i, buffer);
    }
}
