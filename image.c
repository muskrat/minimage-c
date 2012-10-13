/* Simple digital image program
 *
 * I K Stead, 13-10-2012
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VERSTRING   "Isaac's image format, v0.1"

#define HEADER_LEN  32  /* Byte length of whole header field */
#define D_HEAD_LEN  16  /* Byte length of x and y dimension headers */

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

Pixel **pixel_array_rand(int xd, int yd)
{
    int x, y, i;
    Pixel newpixel;
    
    /* Dynamically allocate 2D pixel array */
    Pixel **newarray = malloc(sizeof *newarray * yd);
    for (i = 0; i < yd; i++) {
        newarray[i] = malloc(sizeof *newarray[i] * xd);
    }
    printf("OK\n");

    for (y = 0; y < yd; y++) {
        for (x = 0; x < xd; x++) {
            newarray[x][y] = pixel_new(randbyte(), randbyte(), randbyte());
        }
    }
    return newarray;
}

Image *image_new(int x_size, int y_size)
{
    Image *newimage = malloc(sizeof(Image));
    newimage->x = x_size;
    newimage->y = y_size;
    newimage->pixels = pixel_array_rand(x_size, y_size);

    return newimage;
}

int save(Image *image, char *filename)
{
    FILE *fp;
    Pixel *pixelbuf;
    int x, y;
    
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Could not open %s for writing", filename);
        return EXIT_FAILURE;
    }

    /* Write x and y size into header */
    int hbuf[] = {image->x, image->y};
    fwrite(hbuf, sizeof(int), 2, fp);
    
    /* Read the 2D pixel array into a 1D buffer */
    pixelbuf = malloc((image->x * image->y) * sizeof(Pixel));
    for (y = 0; y < image->y; y++) {
        for (x = 0; x < image->x; x++) {
            pixelbuf[x+y] = image->pixels[x][y];
        }
    }
    
    /* Write the 3-byte pixel values from buffer into file */
    fseek(fp, HEADER_LEN, SEEK_SET);    /* Set file pos to end of header */
    for (y = 0; y < image->y; y++) {
        fwrite(pixelbuf, sizeof(Pixel), image->x, fp);
    }

    fclose(fp);
    free(pixelbuf);
    
    return EXIT_SUCCESS;
}

//Image *load(char *filename)
//{
    /* Read file headers */

    /* Set position past headers */

    /* Read pixels from file into 1D buffer */

    /* Read pixels from 1D buffer into a malloc'd 2D array */

    /* Plug headers values and pointer to pix array into an Image struct and return */
//}

int main()
{
    srand(time(0)); /* Initialise random number generator */

    Image *image = image_new(20, 20);
    printf("OK\n");
    save(image, "crips.iif");

    return EXIT_SUCCESS;
}
