#include <math.h>
#include <inttypes.h>

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
    image_t* image;
    image->relevant_pixels[res];

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
        colour_t r = ((img[i] >> 11) & 0b11111) << 3;
        colour_t g = ((img[i] >> 5) & 0b111111) << 2;
        colour_t b = ((img[i]) & 0b11111) << 3;

        // if value exceeds threshold, increment the counter
        if (r > LOWER_THRESHOLD) r_count++;
        if (g > LOWER_THRESHOLD) g_count++;
        if (b > LOWER_THRESHOLD) b_count++;

        // add each entry into array for further processing
        reds[i] = r;
        greens[i] = g;
        blues[i] = b;

        //printf("r: %i, g: %i, b: %i\n", r, g, b);
    }

    // get average value of the RGB
    // colour_t avg_r = avg(reds, res, r_count);
    // colour_t avg_g = avg(greens, res, g_count);
    // colour_t avg_b = avg(blues, res, b_count);

    // printf("avgs - r: %i, g: %i, b: %i\n", avg_r, avg_g, avg_b);

    // determine which colour is dominant in image
    // RED DOMINANCE
    //if (avg_r > avg_g && avg_r > avg_b) {
    if (r_count > b_count && r_count > g_count) {
    	printf("major red\n");
        image->colour = RED;
        for (i = 0; i < res / 8; i++) {
            image->relevant_pixels[i] = relevant_pixels(reds, i);
        }
    }
    // GREEN DOMINANCE
    // else if (avg_g > avg_r && avg_g > avg_b) {
    else if (g_count > r_count && g_count > b_count) {
    	printf("major green\n");
    	image->colour = LIME;
        for (i = 0; i < res / 8; i++) {
            image->relevant_pixels[i] = relevant_pixels(greens, i);
        }
    }
    // BLUE DOMINANCE
    // else if (avg_b > avg_r && avg_b > avg_g) {
    else if (b_count > r_count && b_count > g_count) {
    	printf("major blue\n");
    	image->colour = BLUE;
        for (i = 0; i < res / 8; i++) {
            image->relevant_pixels[i] = relevant_pixels(blues, i);
        }
    }
    else {
        image->colour = BLACK;
        for (i = 0; i < res / 8; i++) {
            image->relevant_pixels[i] = relevant_pixels(reds, i) | relevant_pixels(greens, i) | relevant_pixels(blues, i);
        }
    }
    return image;
}

/*
 * Determine if 8 pixels in the buffer will be relevant in the drawing
 */
char relevant_pixels(colour_t* col, int offset) {
    char rp = 0;
    int i;
    for (i = 0; i < 8; i++) {
        if (col[offset * 8 + i] >= LOWER_THRESHOLD) {
            rp |= (1 << (7 - i));
        }
    }
    return rp;
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