prefix=/usr/local

all:
	gcc main.c -g -lcurses -lxcb-randr -lxcb-render -lxcb-util -lxcb -o ncxbacklight -Wall

install: ncxbacklight
	sudo cp ncxbacklight $(prefix)/bin/ncxbacklight
