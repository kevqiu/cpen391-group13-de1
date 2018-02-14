#include <math.h>

#include "image_processor.h"
#include "graphics.h"

image_t* process_image(colour_t* img, int res) {
    image_t* image;
    image->relevant_pixels[res];

    colour_t reds[res];
    colour_t greens[res];
    colour_t blues[res];
    int pixel_count = 0;

    int i;
    for (i = 0; i < res; i++) {
        // extract RGB values from 16 bit colour range,
        colour_t r = (img[i] >> 11) & 0b11111 << 3;
        colour_t g = (img[i] >> 5) & 0b111111 << 2;
        colour_t b = (img[i]) & 0b11111 << 3;

        // increment count if pixel contains non-black value
        if (r > LOWER_BOUND || g > LOWER_BOUND || b > LOWER_BOUND) {
            pixel_count++;
        }

        // add each entry into array for further processing
        reds[i] = r;
        greens[i] = g;
        blues[i] = b;
    }

    // get average value of the RGB
    colour_t avg_r = avg(reds, pixel_count);
    colour_t avg_g = avg(greens, pixel_count);
    colour_t avg_b = avg(blues, pixel_count);

    printf("avgs - r: %i, g: %i, b: %i\n", avg_r, avg_g, avg_b);

    // determine which colour is dominant in image
    // RED DOMINANCE
    if (avg_r > avg_g && avg_r > avg_b) {
    	printf("major red\n");
        image->colour = RED;
        for (i = 0; i < res / 8; i++) {
            image->relevant_pixels[i] = relevant_pixels(reds, avg_r, i);
        }
    }
    // GREEN DOMINANCE
    else if (avg_g > avg_r && avg_g > avg_b) {
    	printf("major green\n");
    	image->colour = LIME;
        for (i = 0; i < res / 8; i++) {
            image->relevant_pixels[i] = relevant_pixels(greens, avg_g, i);
        }
    }
    // BLUE DOMINANCE
    else if (avg_b > avg_r && avg_b > avg_g) {
    	printf("major blue\n");
    	image->colour = BLUE;
        for (i = 0; i < res / 8; i++) {
            image->relevant_pixels[i] = relevant_pixels(blues, avg_b i);
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

colour_t avg(colour_t* arr, int count) {
    int sum = 0;
    int i;
    for(i = 0; i < sizeof(arr)/sizeof(colour_t); i++) {
        sum += arr[i];
    }
    return (sum/count);
}

char relevant_pixels(colour_t* col, int avg_val, int offset) {
    char rp = 0;
    int i;
    for (i = 0; i < 8; i++) {
        if (col[offset * 8 + i] >= avg_val) {
            rp |= (1 << (7 - i));
        }
    }
    return rp;
}
