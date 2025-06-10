##
## EPITECH PROJECT, 2025
## Zappy Server Makefile
## File description:
## Makefile
##

SRC	=	src/server.c    \
    	main.c	\
		src/parse_arg.c	\

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
