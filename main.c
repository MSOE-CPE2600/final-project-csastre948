#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h> //used to enable parsing command line arguments 
#include "image_processing.h"


//prints instructions on how to use the program 
void print_usage() {
    printf("Usage: ./image_pipeline -i <input_file> -o <output_file> -e <enhancement_type> [-e <enhancement_type> ...]\n");
    printf("Enhancement types: brightness, contrast, sharpen\n");
}

int main(int argc, char *argv[]) {
    char *input_file = NULL, *output_file = NULL; //stores the names of the input and output images files via -i & -o
    char *enhancements[10]; // Array to store multiple enhancements (max 10) (e.g., brightness, contrast...)
    int enhancement_count = 0; // keeps track of enhancements 
    int opt; //used by getopt to parse command-line arguments

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, "i:o:e:")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'e':
                if (enhancement_count < 10) {
                    enhancements[enhancement_count++] = optarg;
                } else {
                    fprintf(stderr, "Error: Too many enhancements specified.\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                print_usage();
                return EXIT_FAILURE;
        }
    }

    //validation of arguments 

    if (!input_file || !output_file || enhancement_count == 0) {
        print_usage();
        return EXIT_FAILURE;
    }

    //reading the input image 
    Image *img = read_image(input_file); //read image and returns pointer to an image structure containing the image data
    if (!img) {
        fprintf(stderr, "Error: Failed to load image '%s'\n", input_file);
        return EXIT_FAILURE;
    }

    printf("Image dimensions: %dx%d, Channels: %d\n", img->width, img->height, img->channels);


    // Apply each enhancement in sequence
    for (int i = 0; i < enhancement_count; i++) {
        printf("Applying enhancement: %s\n", enhancements[i]);
        if (strcmp(enhancements[i], "brightness") == 0) { //checks the type of enhancement and applies the corresponding function
            adjust_brightness(img, 50); // Example: increase brightness by 50
        } else if (strcmp(enhancements[i], "contrast") == 0) {
            adjust_contrast(img, 1.5); // Example: increase contrast by 1.5x
        } else if (strcmp(enhancements[i], "sharpen") == 0) {
            sharpen_image(img); // Apply sharpening
        } else {
            fprintf(stderr, "Error: Unknown enhancement type '%s'\n", enhancements[i]);
            free_image(img);
            return EXIT_FAILURE;
        }
    }

    //Saving the enhanced image 
    if (!write_image(output_file, img)) {
        fprintf(stderr, "Error: Failed to save image to '%s'\n", output_file);
        free_image(img);
        return EXIT_FAILURE;
    }


    //cleanup and exit 
    printf("Image processing completed successfully.\n");
    free_image(img);
    return EXIT_SUCCESS;
}
