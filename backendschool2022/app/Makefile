SRC		=	$(addprefix src/,	main.cpp\
								Core.cpp\
								Db.cpp\
								ItemResponce.cpp\
								Item.cpp)

NAME	=	restserver
CC		=	g++
CFLAGS	=	-std=c++11 -lpqxx -lpq -lpthread -I include 

.PHONY			:	all re clean fclean

all				:	$(NAME)

$(NAME)			:	$(SRC) Makefile
					@echo "don't panic! it's compiling now"
					@$(CC) $(SRC) $(CFLAGS) -o $(NAME) && echo "compiled"

test			: all
					cd build
					cmake ..
					make 
					./mytests

clean			:
					rm -f $(NAME)

x				:	all
					./$(NAME)

re				:	clean all
