all: run

compile: main.c
	@gcc main.c -o main -std=c99 -Wall -Werror -g

run: compile
	@./main
