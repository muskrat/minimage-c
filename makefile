all:
	clang `sdl-config --cflags --libs` -o minimage image.c
