
#include "server.h"
#include "actions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void update_player_actions(server_t *server)
{
    for (int i = 0; i < server->player_nb; i++) {
        player_t *player = server->players[i];
        action_t *action = player->action_queue;
        if (!action)
            continue;

        action->remaining_ticks--;
        if (action->remaining_ticks <= 0) {
            execute_action(server, player, action);
            player->action_queue = action->next;
            free(action->command);
            free(action);
        }
    }
}
