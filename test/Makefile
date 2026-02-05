# Makefile for minilibx_raylib test suite

NAME = test_suite
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=gnu99 -g

# Raylib paths
RAYLIB_INC = /usr/local/include
RAYLIB_LIB = /usr/local/lib

# Source files
SRCS = test_suite.c
OBJS = $(SRCS:.c=.o)

# Library
MLX_LIB = ../libmlx.a

# Raylib linking
LDFLAGS = -L$(RAYLIB_LIB) -lraylib -lm -lpthread -ldl

all: $(NAME)

$(NAME): $(OBJS) $(MLX_LIB)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(MLX_LIB) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -I.. -I$(RAYLIB_INC) -c $< -o $@

$(MLX_LIB):
	$(MAKE) -C ..

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
