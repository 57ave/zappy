##
## EPITECH PROJECT, 2025
## Zappy Gui Makefile
## File description:
## Makefile
##

SRC	=	main.cpp			\
		src/gui.cpp			\
		src/pars_args.cpp	\
		src/pars_server.cpp	\
		src/Map.cpp			\
		src/Tile.cpp		\
		src/Player.cpp		\
		src/Egg.cpp			\


NAME	=	zappy_gui

OBJ	=	$(SRC:.cpp=.o)

CC	=	g++

CFLAGS	=	-Wall -Wextra

SFMLFLAGS =	-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

all:	$(NAME)
$(NAME):	$(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(SFMLFLAGS)

check-sfml:
	@echo "Checking for SFML dependencies..."
	@if ! pkg-config --exists sfml-graphics; then \
		echo "Error: SFML development libraries not found"; \
		echo "Install with:"";\
		echo " sudo apt-get install libsfml-dev (on Debian/Ubuntu)"; \
		echo "Or: sudo dnf install SFML-devel (on Fedora)"; \
		echo "Or: brew install sfml (on macOS with Homebrew)"; \
		exit 1; \
	fi
	@echo "SFML dependencies found!"

install-deps-debian:
	sudo apt-get update
	sudo apt-get install -y libsfml-dev libgtest-dev \
		libboost-all-dev libboost-filesystem-dev libboost-system-dev

install-deps-fedora:
	sudo dnf install -y SFML-devel gtest-devel boost-devel\
		boost-filesystem-devel boost-system-devel

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY: $(NAME) all clean fclean re check-sfml \
	install-deps-debian install-deps-fedora
