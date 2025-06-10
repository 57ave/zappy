##
## EPITECH PROJECT, 2025
## Zappy Gui Makefile
## File description:
## Makefile
##

SRC	=	main.cpp			\
		src/gui.cpp			\
		src/pars_args.cpp	\

NAME	=	zappy_gui

OBJ	=	$(SRC:.cpp=.o)

CC	=	g++

CFLAGS	=	-Wall -Wextra 

all:	$(NAME)
$(NAME):	$(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY: $(NAME) all clean fclean re
