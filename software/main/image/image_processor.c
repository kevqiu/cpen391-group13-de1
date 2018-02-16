#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "image_processor.h"
#include "graphics.h"

/*
 * Processes an image in 16 bit colour space to determine the majority colour of an image
 * and the relevant pixels to be drawn in that colour.
 * In:
 *  img: array of 16 bit values representing the image
 *  res: resolution of the image, representing x*y
 * Out:
 *  image_t*: image_t struct containing the majority colour and relevant pixels
 */
image_t* process_image(colour_t* img, int res) {
    image_t* image = malloc(sizeof(image_t));
    image->colour_data = malloc(res*sizeof(image_t));

    // initialize colour buffers and counts
    colour_t reds[res];
    colour_t greens[res];
    colour_t blues[res];
    int r_count = 1,
        g_count = 1,
        b_count = 1;

    int i;
    for (i = 0; i < res; i++) {
        // extract RGB values from 16 bit colour range
        colour_t r = ((img[i] >> 11) & 0b11111) << 5;
        colour_t g = ((img[i] >> 5) & 0b111111) << 5;
        colour_t b = ((img[i]) & 0b11111) << 6;

        // if value exceeds threshold, increment the counter
        if (r > LOWER_THRESHOLD) r_count++;
        if (g > LOWER_THRESHOLD) g_count++;
        if (b > LOWER_THRESHOLD) b_count++;

        // add each entry into array for further processing
        reds[i] = r;
        greens[i] = g;
        blues[i] = b;

        //printf("r: %i, g: %i, b: %i\n", r, g, b);
        //printf("r: %i\n", r);
    }
    //printf("r: %i, g: %i, b: %i \n", r_count, g_count, b_count);
    if ((r_count > b_count) && (r_count > g_count)) {
        image->colour = RED;
        memcpy(image->colour_data, reds, 2*res);
    }
    else if (g_count > r_count && g_count > b_count) {
        image->colour = LIME;
        memcpy(image->colour_data, greens, 2*res);
    }
    else if (b_count > r_count && b_count > g_count) {
        image->colour = BLUE;
        memcpy(image->colour_data, blues, 2*res);
    } else {
        printf("You can't be here?\n");
    }
	printf("Returning image: %d\n", image->colour);
    return image;
}

/*
 * Processes an image in 8 bit colour space to determine the majority colour of an image
 * and the relevant pixels to be drawn in that colour.
 * In:
 *  img: array of 8 bit values representing the image
 *  res: resolution of the image, representing x*y
 * Out:
 *  image_t*: image_t struct containing the majority colour and relevant pixels
 */
image8_t* process_8_bit_image(int* img, int res) {
    image8_t* image = malloc(sizeof(image8_t));
    image->colour_data = malloc(res*sizeof(image8_t));

    // initialize colour buffers and counts
    int reds[res];
    int greens[res];
    int blues[res];
    int r_count = 1,
        g_count = 1,
        b_count = 1;

    int i;
    for (i = 0; i < res; i++) {
    	int pixel = img[i];
        // extract RGB values from 16 bit colour range
        int r = (pixel >> 5) & 0b111;
        int g = (pixel >> 2) & 0b111;
        int b = (pixel & 0b11) << 1;

        // if value exceeds threshold, increment the counter
        if (r < LOWER_8_BIT_THRESHOLD) r_count++;
        if (g < LOWER_8_BIT_THRESHOLD) g_count++;
        if (b < LOWER_8_BIT_THRESHOLD) b_count++;

        // add each entry into array for further processing
        reds[i] = r;
        greens[i] = g;
        blues[i] = b;

    }
    //printf("R: %d, G: %d, B: %d\n", r_count, g_count, b_count);
    if ((r_count > b_count) && (r_count > g_count)) {
        image->colour = RED;
        memcpy(image->colour_data, reds, res);
    }
    else if (g_count > r_count && g_count > b_count) {
        image->colour = LIME;
        memcpy(image->colour_data, greens, res);
    }
    else if (b_count > r_count && b_count > g_count) {
        image->colour = BLUE;
        memcpy(image->colour_data, blues, res);
    } else {
        image->colour = BLACK;
        //printf("You can't be here?\n");
    }
	//printf("Returning image: %d\n", image->colour);
    return image;
}

void calculate_relevant_pixels(colour_t* in, char* pixels, int res)
{
    int i;
    for(i = 0; i < res/8; i++) {
        char rp = 0;
        int j;
        for (j = 0; j < 8; j++) {
            if (in[i * 8 + j] >= LOWER_THRESHOLD) {
                rp |= (1 << (7 - j));
            }
        }
        pixels[i] = rp;
    }
}

void calculate_relevant_8_bit_pixels(int* in, char* pixels, int res)
{
    int i;
    for(i = 0; i < res/8; i++) {
        char rp = 0;
        int j;
        for (j = 0; j < 8; j++) {
            if (in[i * 8 + j] >= LOWER_THRESHOLD) {
                rp |= (1 << (7 - j));
            }
        }
        pixels[i] = rp;
    }
}

colour_t avg(colour_t* arr, int arr_len, int count) {
    uint32_t sum = 0;
    int i;
    for(i = 0; i < arr_len; i++) {
        sum += arr[i];
    }
    printf("count: %i. sum:", count);
    printf("%" PRId32, sum);
    printf("\n");

    return (sum/count);
}
