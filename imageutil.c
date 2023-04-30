
// // define STB_IMAGE_IMPLEMENTATION to enable implementation of stb_image functions
// //#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// // define STB_IMAGE_WRITE_IMPLEMENTATION to enable implementation of stb_image_write functions
// //#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// // define STB_IMAGE_RESIZE_IMPLEMENTATION to enable implementation of stb_image_resize functions
// // #define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "imageutil.h"

// Initialize an imatrix from an image file on disk
imatrix* init_from_file(char* image_path, int* width, int* height, int* channels){
    // int i,j;

    // Load the image from the file using stb_image library
    uint8_t* rgb_image;
    rgb_image = stbi_load(image_path, width, height, channels, CHANNEL_NUM);
    // printf("init_from_file:: image_path: %s\n\t", image_path);
    // printf("init_from_file:: width: %d, height: %d, channels: %d\n", *width, *height, *channels);

    // Allocate memory for the imatrix and initialize function pointers
    imatrix* image_matrix = malloc(sizeof(imatrix));
    init_funcptrs(image_matrix);

    // Set the internal RGB image reference and initialize the imatrix data structure
    image_matrix->rgb_image = rgb_image;
    image_matrix = init_rgb(image_matrix, *width, *height);

    // Write the image data to the imatrix
    image_matrix->write_image_to_rgb(image_matrix);    

    return image_matrix;
}

// Initialize an imatrix from an RGB byte array
imatrix* init_from_rgb_image(uint8_t* rgb_image, int width, int height){

    // If the input array is null, return null
    if (rgb_image==NULL)
        return NULL;

    // Allocate memory for the imatrix and initialize function pointers
    imatrix* image_matrix = malloc(sizeof(imatrix));
    init_funcptrs(image_matrix);

    // Set the internal RGB image reference and initialize the imatrix data structure
    image_matrix->rgb_image = rgb_image;
    image_matrix = init_rgb(image_matrix, width, height);

    // Write the image data to the imatrix
    image_matrix->write_image_to_rgb(image_matrix);    

    return image_matrix;
}

// Initialize an imatrix as a blank RGB image
imatrix* init_blank_rgb_image(int width, int height){

    // Allocate memory for the imatrix and initialize function pointers
    imatrix* image_matrix = malloc(sizeof(imatrix));
    init_funcptrs(image_matrix);

    // Allocate memory for the internal RGB image reference and initialize the imatrix data structure
    image_matrix->rgb_image = (uint8_t*) malloc(sizeof(uint8_t) * width * height * CHANNEL_NUM);
    image_matrix = init_rgb(image_matrix, width, height);
    
    return image_matrix;
}


// Initialize the function pointers of an imatrix
void init_funcptrs(imatrix* this){
    // Declare function pointers for matrix operations and image writing
    imatrix* add(imatrix* m1, imatrix* m2); 
    imatrix* subtract(imatrix* m1, imatrix* m2);
    imatrix* dot(imatrix* m1, imatrix* m2);
    void write_image_to_rgb(imatrix* this);
    void write_rgb_to_image(imatrix* m);
    imatrix* scale(imatrix* this, int width, int height, float alpha);

    // If the input imatrix is null, return
    if (this==NULL)
        return;

    // Set the function pointers of the imatrix
    this->add = add;
    this->subtract = subtract;
    this->dot = dot;
    this->write_image_to_rgb = write_image_to_rgb;
    this->write_rgb_to_image = write_rgb_to_image;
    this->scale = scale;
}

/*
*   Initializes a new RGB imatrix object with the given width, height.
*   @param this the imatrix object to initialize
*   @param width the width of the image
*   @param height the height of the image
*   @param channels the number of color channels in the image (must be 3 for RGB)
*   @returns a reference to the initialized imatrix object
*/
imatrix* init_rgb(imatrix* this, int width, int height){

     this->width = width;
    this->height = height;

    // Allocate memory for the three channel matrices
    // YOUR CODE HERE
    this->r = malloc(sizeof(uint8_t*)*height);
    for(int i=0 ; i<height;i++){
        this->r[i] = malloc(sizeof(uint8_t)* width); 
    }

    this->g = malloc(sizeof(uint8_t*)*height);
    for(int j=0 ; j<height;j++){
        this->g[j] = malloc(sizeof(uint8_t)* width); 
    }

    this->b = malloc(sizeof(uint8_t*)*height);
    for(int k=0 ; k<height;k++){
        this->b[k] = malloc(sizeof(uint8_t)* width); 
    }
    
    return this;
}

// Set the pixel data of an imatrix from an RGB byte array
imatrix* set_rgb_image(imatrix* this, uint8_t* new_rgb_image, int height, int width){
    // Free any memory previously allocated for the imatrix and initialize from the new RGB image
    free_imatrix(this);
    return init_from_rgb_image(new_rgb_image, height, width);
}


// Write the pixel data of an imatrix to its internal RGB image buffer
void write_rgb_to_image(imatrix* m){
    int i,j, height, width;
    height = m->height;
    width = m->width;

    // Iterate through the image matrix and set the corresponding RGB pixel values in the RGB buffer
    for (i=0; i<height; ++i){
        for(j=0; j<width; ++j){
            *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + RED) = m->r[i][j];
            *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + GREEN) = m->g[i][j];
            *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + BLUE) = m->b[i][j];
        }
    }
}

// Write the pixel data of the internal RGB image buffer to an imatrix
void write_image_to_rgb(imatrix* m){
    int i,j, height, width;
    height = m->height;
    width = m->width;

    // Iterate through the RGB buffer and set the corresponding pixel values in the image matrix
    for (i=0; i<height; ++i){
        for(j=0; j<width; ++j){
            m->r[i][j] = *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + RED);
            m->g[i][j] = *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + GREEN);
            m->b[i][j] = *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + BLUE);
        }
    }
}

// Free all allocated memory for an imatrix
void free_imatrix(imatrix* image_matrix){
    int i;

    // If the input imatrix is null, return
    if (image_matrix==NULL)
        return;  

    // Free the memory allocated for this->r, this->g, and this->b
    for (i = 0; i < image_matrix->height; ++i) {
        if (image_matrix->r[i] != NULL){
            free(image_matrix->r[i]);
            image_matrix->r[i] = NULL;
        }
        if (image_matrix->g[i] != NULL){
            free(image_matrix->g[i]);
            image_matrix->g[i] = NULL;
        }
        if (image_matrix->b[i] != NULL){
            free(image_matrix->b[i]);
            image_matrix->b[i] = NULL;
        }
    }

    // // Free the memory allocated for image_matrix->rgb_image
    if (image_matrix->rgb_image != NULL){
        free(image_matrix->rgb_image);
        image_matrix->rgb_image = NULL;
    }
}




/*
*   Add two matrices together. Creates a new imatrix object and returns a reference to it. Returns
*   NULL if the matrices have different sizes.
*
*   @param m1 the imatrix object for the first image
*   @param m2 the imatrix object for the second image
*   @returns a reference to a newly allocated imatrix object that is the clipped sum of the two input images or
*                NULL if the matrices are not the same size (same number of rows and columns).
*            Note: This memory must be freed when you're done using it.
*/
imatrix* add(imatrix* m1, imatrix* m2){
    
    if (m1->height != m2->height || m1->width != m2->width)
        return NULL;

    // Allocate memory for the output matrix.
    imatrix* result = init_blank_rgb_image(m1->width, m1->height);
    // YOUR CODE HERE

    for(int i=0; i < (m1->height);i++){
        for(int j= 0;j < (m1->width);j++){
            result->r[i][j] = (m1->r[i][j]) + (m2->r[i][j]);
            result->b[i][j] = (m1->b[i][j]) + (m2->b[i][j]);
            result->g[i][j] = (m1->g[i][j]) + (m2->g[i][j]);
        }

    }
    // FILL IN THE CODE HERE
    return result;
}



/*
*   Subtract m2 from m1. Creates a new imatrix object and returns a reference to it. Returns
*   NULL if the matrices have different sizes.
*
*   @param m1 the imatrix object for the first image
*   @param m2 the imatrix object for the second image
*   @returns a reference to a newly allocated imatrix object that is the clipped difference of the two input images or
*                NULL if the matrices are not the same size (same number of rows and columns).
*            Note: This memory must be freed when you're done using it.
*/
imatrix* subtract(imatrix* m1, imatrix* m2) {
    // FILL IN THE CODE HERE
    if (m1->height != m2->height || m1->width != m2->width)
        return NULL;

  imatrix* result = init_blank_rgb_image(m1->width, m1->height);
    // YOUR CODE HERE

    for(int i=0; i < (m1->height);i++){
        for(int j= 0;j < (m1->width);j++){
            result->r[i][j] = (m1->r[i][j]) - (m2->r[i][j]);
            result->b[i][j] = (m1->b[i][j]) - (m2->b[i][j]);
            result->g[i][j] = (m1->g[i][j]) - (m2->g[i][j]);
        }
    }
    return result;
}


/*
*   Matrix multiplication. Multiplies m1*m2 using matrix-matrix dot. Returns a new imatrix object with
*   the output. If m1 is a (m, n) matrix, then m2 must be a (n, k) matrix for any value of k >=1.
*
*   @param m1 the left matrix.
*   @param m2 the right matrix.
*   @returns a new matrix with the result or NULL of the dimensions do not match properly for matrix multiplications.  
*               Note: This memory must be freed after use.
*/
imatrix* dot(imatrix* m1, imatrix* m2){

    // Check if the dimensions of the matrices are compatible for multiplication.
    if (m1->width != m2->height)
        return NULL;

    // Allocate memory for the output matrix.
    imatrix* result = malloc(sizeof(imatrix));

    // YOUR CODE HERE
    result = init_blank_rgb_image(m1->height, m2->width); //Initialize result

     for (int i = 0; i < m1->height; i++) {
        for (int j = 0; j < m2->width; j++) {
            for (int k = 0; k < m1->width; k++) {
                result->r[i][j] += m1->r[i][k] * m2->r[k][j];
                result->g[i][j] += m1->g[i][k] * m2->g[k][j];
                result->b[i][j] += m1->b[i][k] * m2->b[k][j];
            }
        }
    }

    // Return the output matrix.
    return result;
}


/*
*   Scales all the pixel values of an image by a given factor between 0.0 and 1.0. If the input image or scaling factor
*   are invalid, the function returns a reference to the input image without modification.
*
*   @param this pointer to the input image matrix object to be scaled.
*   @param width the width of the input image.
*   @param height the height of the input image.
*   @param alpha scaling factor between 0.0 and 1.0.
*   @returns a reference to the input image matrix object with all the pixel values scaled by alpha or 
*       NULL if the input image is NULL.
*/
imatrix* scale(imatrix* this, int width, int height, float alpha){
    
    // FILL IN THE CODE HERE
    if (this == NULL)
    {
        return NULL;
    }
    else if (alpha > 0 && alpha < 1)
    {
        for(int i = 0; i < height; i++)
        {  
            for(int j=0; j < width; j++)
            {
                this->r[i][j] = this->r[i][j] * alpha;
                this->g[i][j] = this->g[i][j] * alpha;
                this->b[i][j] = this->b[i][j] * alpha;
            }

        }
    }


    return this;
}



