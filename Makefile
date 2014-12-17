all:
	gcc main.c -lcurses -lxcb-randr -lxcb-render -lxcb-util -lxcb -o ncxbacklight -Wall
