/* Simple digital image program
 *
 * TODO: Add more error checking
 * TODO: Split things up into separate source files and headers
 *
 * I K Stead, 13-10-2012
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>

#define MAX_SIZE    1020

/*
 * Helpers
 */

/* Generate a random byte value */
unsigned char 
randbyte()
{
    int r = rand() % 256;
    return (unsigned char)r;
}

/* Convert an integer to a byte array */
void
int_to_bytes(int n, unsigned char *bytes)
{
    bytes[0] = (n >> 24) & 0xff;
    bytes[1] = (n >> 16) & 0xff;
    bytes[2] = (n >> 8) & 0xff;
    bytes[3] = n & 0xff;
}


/*
 * Data structures
 */

/* Holds RGB byte values of one pixel */
typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Pixel;

/* Holds image pixel dimensions and pointer to pixel array */
typedef struct {
    int x;
    int y;
    Pixel **pixels;
} Image;

/* Allocate memory for a 2D array of pixels */
Pixel **alloc_pixel_array(int size_x, int size_y)
{
    int i;
    Pixel **array;

    array = malloc(sizeof *array * size_y);

    for (i = 0; i < size_y; i++)
        array[i] = malloc(sizeof *array[i] * size_x);

    return array;
}


/*
 * File save / load functions
 */

int read_header(FILE *fp)
{
    unsigned char bytes[4];
    int result, i;

    fread(bytes, 1, 4, fp);

    result = 0;

    for (i = 0; i < 4; i++)
        result += bytes[i];
    
    return result;
}

Image *load(char *filename)
{    
    FILE *fp = fopen(filename, "rb");

    if (!fp) {
        printf("Could not open %s for reading\n", filename);
        return NULL;
    }

    /* Build up the image object */
    Image *image = malloc(sizeof(Image));

    image->x = read_header(fp);
    image->y = read_header(fp);
    image->pixels = alloc_pixel_array(image->x, image->y);

    /* Read 3 byte pixels a row at a time from file into image pixel 
     * array.
     */
    int x, y;
    unsigned char pixbuf[3];

    for (y = 0; y < image->y; y++) {
        for (x = 0; x < image->x; x++) {
            fread(pixbuf, 1, 3, fp);
            image->pixels[x][y].red = pixbuf[0];
            image->pixels[x][y].green = pixbuf[1];
            image->pixels[x][y].blue = pixbuf[2];
        }
    }

    /* Clean up */
    fclose(fp);
    
    return image;
}

int save(Image *image, char *filename)
{
    FILE *fp = fopen(filename, "wb");

    if (!fp)  {
        printf("Could not open %s for writing\n", filename);
        return EXIT_FAILURE;
    }

    /* Write dimensions to file header in bytes */
    unsigned char sizebuf[4];

    int_to_bytes(image->x, sizebuf);
    fwrite(sizebuf, 1, 4, fp);

    int_to_bytes(image->y, sizebuf);
    fwrite(sizebuf, 1, 4, fp);

    int x, y;
    unsigned char pixbuf[3];

    for (y = 0; y < image->y; y++) {
        for (x = 0; x < image->x; x++) {
            pixbuf[0] = image->pixels[x][y].red;
            pixbuf[1] = image->pixels[x][y].green;
            pixbuf[2] = image->pixels[x][y].blue;
            fwrite(pixbuf, 1, 3, fp);
        }
    }

    fclose(fp);

    return EXIT_SUCCESS;
}


/*
 * Image creation and manipulation functions
 */

Image *new_image(int size_x, int size_y)
{
    Image *image = malloc(sizeof(Image));

    image->x = size_x;
    image->y = size_y;
    image->pixels = alloc_pixel_array(size_x, size_y);

    /* Initialise pixels to black */
    int x, y;

    for (y = 0; y < image->y; y++) {
        for (x = 0; x < image->x; x++) {
            image->pixels[x][y].red = 0xff;
            image->pixels[x][y].green = 0xff;
            image->pixels[x][y].blue = 0xff;
        }
    }

    return image;
}

int randomise_image(Image *image)
{
    int x, y;

    for (y = 0; y < image->y; y++) {
        for (x = 0; x < image->x; x++) {
            image->pixels[x][y].red = randbyte();
            image->pixels[x][y].green = randbyte();
            image->pixels[x][y].blue = randbyte();
        }
    }

    return EXIT_SUCCESS;
}
void display(SDL_Surface *surface, Image *image)
{
    int x, y;
    Pixel current;
    Uint32 colour;

    for (y = 0; y < image->y; y++) {
        for (x = 0; x < image->x; x++) {
            current = image->pixels[x][y];
            colour = SDL_MapRGB(surface->format, current.red, current.green, current.blue);
            SDL_Rect pixel = {x, y, 1, 1};
            SDL_FillRect(surface, &pixel, colour);
        }
    }
    return;
}

int main()
{
    srand(time(0)); /* Initialise random number generator */

//    Image *image = new_image(100, 100);
//    randomise_image(image);

//    save(image, "matemaatonga.iif");
    Image *image = load("matemaatonga.iif");

    /* Set up SDL */
    SDL_Surface *screen;
    SDL_Event event;
    int exit = 0;

    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(image->x, image->y, 32, SDL_SWSURFACE);

    /* Main loop */
    while (!exit) {
        display(screen, image);
        SDL_Flip(screen);   /* Updates SDL window */
        
        /* Check for user input, quit if required */
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit = 1;
                    break;
                case SDL_KEYDOWN:
                    exit = 1;
                    break;
            }
        }
    }
    SDL_Quit();

    return EXIT_SUCCESS;
}
