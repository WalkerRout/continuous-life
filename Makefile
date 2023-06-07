CC = gcc
OBJS = src/*.c
OBJ = bin/continuous-life
CFLAGS = -O3 -Wall -Wextra -Wunused-result -Werror -Wpedantic -Wc++-compat
LIBS = -Iinclude/ -lm

all: run

build:
	@$(CC) $(OBJS) $(CFLAGS) $(LIBS) -o $(OBJ)

run: build
	@./$(OBJ)

clean:
	@rm ./$(OBJ)
	@echo "Cleaned!"
