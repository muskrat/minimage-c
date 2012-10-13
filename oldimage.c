/* File format: first two bytes contain the x and y size of the file in
 * pixels. Remaining bytes encode grayscale pixels
 */
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define MAX_PX  255

typedef struct pixel_struct {
    unsigned char R;
    unsigned char G;
    unsigned char B;
} Pixel;

typedef struct image_struct {
    int size_x;
    int size_y;
    Pixel **pixels;
} Image;

/* Generate a random integer r such that 0 <= r < range */
int randint(int range)
{
    int r = rand() % range;
    return r;
}

/* Print the value of an unsigned char in hex notation */
void printhex(unsigned char byte)
{
}

FILE *create(char *name, int size_x, int size_y)
{
    FILE *fp = fopen(name, "w+");
    unsigned char size[] = {(int)size_x, (int)size_y};
    int w;

    w = fwrite(size, 1, 2, fp);

    return fp;
}

/* Write pixels to image file one row at a time */
void write_pixels(unsigned char *pixels, FILE *image)
{
    int y;
    unsigned char size[2];

    rewind(image);
    fread(size, 1, 2, image);
    /* Set file position to third byte to avoid overwriting headers */
    fseek(image, 2, SEEK_SET);

    /* Iterate through the buffer writing bytes to file one at a time */
    for (y = 0; y < size[1]; y++) {
        fwrite(pixels, sizeof(unsigned char), size[0], image);
    }
}

unsigned char *create_test_pixels(int size_x, int size_y)
{
    unsigned char *pixel_array, pixel;
    int i;
    int size = size_x * size_y;

    pixel_array = malloc(size);

    for (i = 0; i < size; i++) {
        pixel = (unsigned char)randint(256);
        pixel_array[i] = pixel;
    }
    return pixel_array;
}

int main()
{
    FILE *fp = create("gangbang.iif", 10, 10);
    unsigned char *pixels = create_test_pixels(10, 10);
    write_pixels(pixels, fp);
    fclose(fp);

    return EXIT_SUCCESS;
}
