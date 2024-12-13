# Image Enhancement Pipeline

This project implements a basic image enhancing pipeline in C, providing functionality to process PNG images with various enhancements, such as brightness adjustment, contrast adjustment, and sharpening. The pipeline is designed for efficiency and flexibility, allowing multiple enhancements to be applied in a single run.

## Features

- **Brightness Adjustment**: Modify the intensity of pixel values to lighten or darken the image.
- **Contrast Adjustment**: Increase or decrease the difference between light and dark areas in the image.
- **Sharpening**: Enhance edges and details using a convolutional sharpening filter.
- **Sequential Enhancements**: Apply multiple enhancements in the order specified via command-line arguments.


### Build the program 
make

#### Usage 
**Command-Line Arguments**:
./image_pipeline -i <input_file> -o <output_file> -e <enhancement_type> [-e <enhancement_type> ...]

**Options:**
- **-i <input_file>**: Path to the input PNG image file.
- **o <output_file>**: Path to save the processed PNG image.
- **e <enhancement_type>**: Type of enhancement to apply.
Supported types:
- brightness: Increases brightness.
- contrast: Adjusts contrast.
- sharpen: Sharpens the image.
Multiple -e options can be specified to apply enhancements sequentially.

##### Example
Apply sharpening and contrast adjustment:

./image_pipeline -i input.png -o output.png -e sharpen -e contrast

