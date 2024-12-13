#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

typedef struct {
    int width, height, channels;
    unsigned char *data;
} Image;

Image *read_image(const char *filename);
int write_image(const char *filename, const Image *image);
void adjust_brightness(Image *image, int value);
void adjust_contrast(Image *image, float factor);
void sharpen_image(Image *image);
void free_image(Image *image);

#endif
