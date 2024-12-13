/******************************************************************************************************************
* @file image_processing.c
* @brief Image Enhancement Pipeline with Brightness, Contrast, and Sharpening Features
*
* Course: CPE2600
* Section: 111
* Assignment: CPE Lab 13
* Author: Carla Sastre
*
* Description: This program processes PNG images by applying various enhancements such as brightness adjustment, 
*              contrast adjustment, and sharpening. Multiple enhancements can be applied sequentially via 
*              command-line arguments.
* Date: 12/11/2024
*
* Compile Instructions:
*   To compile this file, use:
*   - make
*   Or manually:
*   - gcc -o image_pipeline main.c image_processing.c -lpng -lm
******************************************************************************************************************/


#include "image_processing.h"
#include <png.h> //using libpng library 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//function to read image
Image *read_image(const char *filename) { //read png image from disk
    FILE *fp = fopen(filename, "rb"); //opens file in binary read mode
    if (!fp) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }

    //checks png signature 
    unsigned char header[8];
    if (fread(header, 1, 8, fp) != 8 || png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "Error: File '%s' is not a valid PNG\n", filename);
        fclose(fp);
        return NULL;
    }

    //create png strucures 
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Error: png_create_read_struct failed\n");
        fclose(fp);
        return NULL;
    }

    png_infop info = png_create_info_struct(png); //creates an info structure to store metadata about the png (width, height...)
    if (!info) {
        fprintf(stderr, "Error: png_create_info_struct failed\n");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return NULL;
    }

    //handle png errors
    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error: libpng read error\n");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return NULL;
    }

    //initialize PNG I/O
    png_init_io(png, fp); //Links the PNG read structure to the file pointer.
    png_set_sig_bytes(png, 8); //Informs libpng that 8 signature bytes were already read.
    png_read_info(png, info);//Reads the PNG's metadata (dimensions, channels, etc.).


    //Allocate memory for the image
    Image *image = (Image *)malloc(sizeof(Image));
    image->width = png_get_image_width(png, info);
    image->height = png_get_image_height(png, info);
    image->channels = png_get_channels(png, info);

    size_t row_size = image->width * image->channels;
    image->data = (unsigned char *)malloc(row_size * image->height);
    png_bytep *rows = (png_bytep *)malloc(sizeof(png_bytep) * image->height);
    for (int y = 0; y < image->height; y++) {
        rows[y] = &image->data[y * row_size];
    }


    //read pixel data 
    png_read_image(png, rows);
    free(rows);

    //cleanup
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);

    return image;
}


//function to write image to a png file
int write_image(const char *filename, const Image *image) {
    FILE *fp = fopen(filename, "wb"); //open file
    if (!fp) {
        fprintf(stderr, "Error: Could not open file '%s' for writing\n", filename);
        return 0;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); //initializes png writing
    if (!png) {
        fprintf(stderr, "Error: png_create_write_struct failed\n");
        fclose(fp);
        return 0;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Error: png_create_info_struct failed\n");
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return 0;
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error: libpng write error\n");
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return 0;
    }

    png_init_io(png, fp);


    //set png metadata, configures the image properties 
    png_set_IHDR(png, info, image->width, image->height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    size_t row_size = image->width * image->channels; //write pixel data
    png_bytep *rows = (png_bytep *)malloc(sizeof(png_bytep) * image->height);
    for (int y = 0; y < image->height; y++) {
        rows[y] = &image->data[y * row_size];
    }

    png_set_rows(png, info, rows);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    free(rows);
    
    //cleanup, free the structures and closes the file
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    return 1;
}


//adjust brightness, add value to each pixel and clamps between 0 and 255
void adjust_brightness(Image *image, int value) {
    for (int i = 0; i < image->width * image->height * image->channels; i++) {
        int new_value = image->data[i] + value;
        image->data[i] = (new_value > 255) ? 255 : (new_value < 0) ? 0 : new_value;
    }
}


//adjust contrast, multiplies pixel values relative to 128 to increase and decrease contrast 
void adjust_contrast(Image *image, float factor) {
    for (int i = 0; i < image->width * image->height * image->channels; i++) {
        int new_value = (int)((image->data[i] - 128) * factor + 128);
        image->data[i] = (new_value > 255) ? 255 : (new_value < 0) ? 0 : new_value;
    }
}


//adjust sharpening
void sharpen_image(Image *image) {
    unsigned char *original_data = (unsigned char *)malloc(image->width * image->height * image->channels);
    memcpy(original_data, image->data, image->width * image->height * image->channels);

    // Define a normalized 3x3 sharpening kernel
    int kernel[3][3] = {
        {  0,  -1,   0 },
        { -1,   5,  -1 },
        {  0,  -1,   0 }
    };
    int kernel_size = 3;
    int half_kernel = kernel_size / 2;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            for (int c = 0; c < image->channels; c++) {
                int pixel_sum = 0;

                // Apply the kernel
                for (int ky = -half_kernel; ky <= half_kernel; ky++) {
                    for (int kx = -half_kernel; kx <= half_kernel; kx++) {
                        int pixel_y = y + ky;
                        int pixel_x = x + kx;

                        // Check if the pixel is within bounds
                        if (pixel_y >= 0 && pixel_y < image->height && pixel_x >= 0 && pixel_x < image->width) {
                            int original_index = (pixel_y * image->width + pixel_x) * image->channels + c;
                            pixel_sum += kernel[ky + half_kernel][kx + half_kernel] * original_data[original_index];
                        }
                    }
                }

                // Clamp the pixel value between 0 and 255
                int output_index = (y * image->width + x) * image->channels + c;
                image->data[output_index] = (pixel_sum > 255) ? 255 : (pixel_sum < 0) ? 0 : pixel_sum;
            }
        }
    }

    free(original_data);
}



//free image, frees memory for pixel data and the image structure
void free_image(Image *image) {
    free(image->data);
    free(image);
}
