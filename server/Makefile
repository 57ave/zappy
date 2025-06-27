##
## EPITECH PROJECT, 2025
## Zappy Server Makefile
## File description:
## Makefile
##

SRC	=	src/server.c    \
		src/handle_client.c	\
    	main.c	\
		src/parse_arg.c	\
		src/map.c	\
		src/player.c	\
<<<<<<< HEAD
		src/commands.c	\
		src/action.c	\
		src/calculate.c	\
		src/count.c	\
		src/create.c	\
		src/dynamic.c	\
		src/eject.c	\
		src/find.c	\
		src/inventory.c	\
		src/look.c	\
		src/tile.c	\
    	src/send_data_gui.c	\
		src/position.c	\
		src/queue.c	\
		src/execute.c	\
		src/actions_queue.c	\
		src/handle_commands.c	\
		src/handle_update.c	\
		src/loop.c	\
		src/tick.c	\
		src/connection.c	\
		src/process_client.c	\
		src/response.c	\
		src/new_pos.c	\
=======
		src/send_data_gui.c	\
>>>>>>> main

OBJ	=	$(SRC:.c=.o)

NAME	=	zappy_server

CC	=	gcc

CFLAGS	=	-Wall -Wextra -g

CPPFLAGS =  -I ./include/

all: $(NAME)

$(NAME):	$(OBJ)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY: $(NAME) all clean fclean re
