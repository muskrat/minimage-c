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

#define HEADER_LEN  32  /* Byte length of whole header field */

/* Generate a random byte value */
unsigned char randbyte()
{
    int r = rand() % 256;
    return (unsigned char)r;
}

/* Holds RGB byte values of one pixel */
typedef struct pixel_struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Pixel;

/* Holds image pixel dimensions and pointer to pixel array */
typedef struct image_struct {
    int x;
    int y;
    Pixel **pixels;
} Image;

/* Testing functions */

Pixel pixel_new(unsigned char r, unsigned char g, unsigned char b)
{
    Pixel newpix = {r, g, b};
    return newpix;
}

Pixel **pixel_array(int xd, int yd, int random)
{
    int x, y, i;
    Pixel newpixel;
    
    /* Dynamically allocate 2D pixel array */
    Pixel **newarray = malloc(sizeof *newarray * yd);
    for (i = 0; i < yd; i++) {
        newarray[i] = malloc(sizeof *newarray[i] * xd);
    }

    /* Write random byte values to pixels if random option set */
    if (rand) {
        for (y = 0; y < yd; y++) {
            for (x = 0; x < xd; x++) {
                newarray[x][y] = pixel_new(randbyte(), randbyte(), randbyte());
            }
        }
    }
    return newarray;
}

Image *image_new(int x_size, int y_size, Pixel **pixels)
{
    Image *newimage = malloc(sizeof(Image));
    newimage->x = x_size;
    newimage->y = y_size;
    newimage->pixels = pixels;

    return newimage;
}

int image_discard(Image *image)
{
    int i;

    for (i = 0; i < image->y; i++)
        free(image->pixels[i]);
    free(image->pixels);
    free(image);

    return EXIT_SUCCESS;
}

int save(Image *image, char *filename)
{
    int x, y;
    FILE *fp;
    Pixel *pixelbuf;

    fp = fopen(filename, "w");
    if (!fp)  {
        printf("Could not open %s for writing\n", filename);
        return EXIT_FAILURE;
    }

    /* Write x and y size into header */
    int hbuf[] = {image->x, image->y};
    fwrite(hbuf, sizeof(int), 2, fp);
    
    /* Read the 2D pixel array into a 1D buffer in rows */
    pixelbuf = malloc((image->x * image->y) * sizeof(Pixel));
    for (y = 0; y < image->y; y++) {
        for (x = 0; x < image->x; x++) {
            pixelbuf[x+y] = image->pixels[x][y];
        }
    }
    
    printf("Pixel buffer size %li bytes\n", sizeof(*pixelbuf));

    /* Write the 3-byte pixel values from buffer into file */
    fseek(fp, HEADER_LEN, SEEK_SET);    /* Set file pos to end of header */
    for (y = 0; y < image->y; y++) {
        fwrite(pixelbuf, sizeof(Pixel), image->x, fp);
    }

    fclose(fp);
    free(pixelbuf);
    
    return EXIT_SUCCESS;
}

/* Read an image from disk into a disk object in memory and return reference.
 * FIXME: all the hbuf[0], hbuf[1] is pretty unreadable, assign them to vars
 * FIXME: bug in here somewhere, pixels possibly being read in wrong order
 */
Image *load(char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (!fp) {
        printf("Could not open %s for reading\n", filename);
        return NULL;
    }

    /* Read x and y sizes from header */
    int hbuf[2];
    fread(hbuf, sizeof(int), 2, fp);

    /* Read pixel values into buffer */
    int x, y;
    Pixel *pixelbuf = malloc((hbuf[0] * hbuf[1]) * sizeof(Pixel));
    fseek(fp, HEADER_LEN, SEEK_SET);    /* Skip headers */

    for (y = 0; y < hbuf[1]; y++)
        fread(pixelbuf, sizeof(Pixel), hbuf[0], fp);
    
    /* Insert pixels from buffer into a 2D array, then create a new image
     * object with all the data we've collected
     */
    Pixel **pixels = pixel_array(hbuf[0], hbuf[1], 0);

    for (y = 0; y < hbuf[1]; y++) {
        for (x = 0; x < hbuf[0]; x++) {
            pixels[x][y] = pixelbuf[x+y];
        }
    }
    Image *image = image_new(hbuf[0], hbuf[1], pixels);

    /* Clean up */
    free(pixelbuf);
    fclose(fp);

    return image;
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

    //Pixel **pixels = pixel_array(300, 300, 1);
    //Image *image = image_new(300, 300, pixels);
    //save(image, "randompixels.iif");
    
    Image *image = load("randompixels.iif");

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

    image_discard(image);

    return EXIT_SUCCESS;
}
