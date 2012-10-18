all:
	clang `sdl-config --cflags --libs` -Wuninitialized -o minimage new.c
