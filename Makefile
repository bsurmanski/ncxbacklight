all:
	gcc main.c -g -lcurses -lxcb-randr -lxcb-render -lxcb-util -lxcb -o ncxbacklight -Wall
