#include "grid_detection.h"
#include <stdio.h>
#include <stddef.h>


//
//
//checks if pixel is black //<128 otherwise white

static int is_black(const unsigned char *image,int width,int height,int stride,int x, int y)
{
    	(void)width;

    	if (x < 0 || y < 0 || x >= width || y >= height)
        	return 0;

    	return image[(size_t)y * (size_t)stride + (size_t)x] < 128;
}



static int longest_black_run_in_row(const unsigned char *image,int width, int height, int stride, int y)
{
    	int x;
    	int current_run = 0;
    	int longest_run = 0;

    	for (x = 0; x < width; x++)
    	{
        	if (is_black(image, width, height, stride, x, y))
        	{
            		current_run++;
            		if (current_run > longest_run)
                		longest_run = current_run;
        	}
        	else
        	{
            		current_run = 0;
        	}
    	}

    	return longest_run;
}



//search for the longest column

static int longest_black_run_in_column(const unsigned char *image,
                                       int width,
                                       int height,
                                       int stride,
                                       int x)
{
    	int y;
    	int current_run = 0;
    	int longest_run = 0;

    	for (y = 0; y < height; y++)
    	{
        	if (is_black(image, width, height, stride, x, y))
        	{
            	current_run++;
            	if (current_run > longest_run)
                	longest_run = current_run;
        	}
        	else
        	{
            		current_run = 0;
        	}
    	}

    return longest_run;
}




//looking for grid coordinates

//stride -> number of bytes between the start of 2 consecutive lines
int detect_grid_bounds(const unsigned char *image,
                       int width,
                       int height,
                       int stride,
                       GridCoordinates *grid)
{
    int x;
    int y;
    int min_run;
    int top = -1;
    int bottom = -1;
    int left = -1;
		 
    int right = -1;
    int best_horizontal_run = 0;
    int best_vertical_run = 0;
    int horizontal_threshold;
    int vertical_threshold;

    if (image == NULL || grid == NULL || width <= 0 || height <= 0 ||
        stride < width)
        return 0;
//minimal black pixel length estimate
    min_run = (width < height ? width : height) / 12;
    if (min_run < 10)
        min_run = 10;

    horizontal_threshold = min_run;
    vertical_threshold = min_run;

    for (y = 0; y < height; y++)
    {
        int run = longest_black_run_in_row(image, width, height, stride, y);

        if (run >= horizontal_threshold)
        {
            		if (top == -1)
                		top = y;
            		bottom = y;

            		if (run > best_horizontal_run)
                		best_horizontal_run = run;
        	}
    	}

    	for (x = 0; x < width; x++)
    	{
        	int run = longest_black_run_in_column(image, width, height, stride, x);

        	if (run >= vertical_threshold)
        	{
            		if (left == -1)
                		left = x;
            		right = x;

            		if (run > best_vertical_run)
                		best_vertical_run = run;
        	}
    	}

    	if (top < 0 || bottom <= top || left < 0 || right <= left)
        	return 0;

    	if (bottom - top < min_run || right - left < min_run)
        	return 0;

    	grid->bounds.top_left.x = left;
    	grid->bounds.top_left.y = top;

    	grid->bounds.top_right.x = right;
    	grid->bounds.top_right.y = top;

    	grid->bounds.bottom_right.x = right;
    	grid->bounds.bottom_right.y = bottom;
	
    	grid->bounds.bottom_left.x = left;
    	grid->bounds.bottom_left.y = bottom;

    	grid->rows = 0;
    	grid->columns = 0;
    	grid->cell_width = 0;
    	grid->cell_height = 0;

    	return 1;
}



int detect_grid_dimensions(const unsigned char *image,
                           int width,
                           int height,
                           int stride,
                           GridCoordinates *grid)
{
	if (image == NULL || grid == NULL ||
    		width <= 0 || height <= 0 || stride < width)
	{

    		return 0;
	}
	int left = grid->bounds.top_left.x;
	int right = grid->bounds.top_right.x;
	int top = grid->bounds.top_left.y;
	int bottom = grid->bounds.bottom_left.y;
	int grid_width = right - left + 1;
	int black_pixels = 0;
	if (left < 0 || right >= width ||
    		top < 0 || bottom >= height ||
    		right <= left || bottom <= top)
	{
    		return 0;
	}
	double black_ratio;
	int horizontal_borders = 0;//counts number of edges detected
	int in_border = 0; //indicates if already in a border
	for (int y=top; y<=bottom; y++)
	{
		black_pixels=0;
		for (int x = left; x <= right; x++)
		{
    			if (image[y * stride + x] < 128)
    			{
        			black_pixels++;
    			}
		}
		black_ratio = (double)black_pixels / grid_width;
		if (black_ratio >= 0.8 )
		{
			if (in_border==0)
			{
				horizontal_borders++;
				in_border=1;
			}

    			
		}
		else{
			in_border=0;
		}
	}
	printf("Bordures horizontales détectées : %d\n",
       horizontal_borders);
	grid->rows = horizontal_borders - 1;
	int vertical_borders = 0;//counts number of edges detected
        int in_border_vertical = 0; //indicates if already in a border
	int grid_height = bottom - top + 1;
	for (int x=left; x<=right; x++)
	{
		black_pixels=0;
		for (int y=top; y<=bottom; y++)
		{
			if (image[y*stride+x]<128)
			{
				black_pixels++;
			}
		}
		black_ratio = (double)black_pixels / grid_height;
		if (black_ratio >= 0.8 )
                {
                        if (in_border_vertical==0)
                        {
                                vertical_borders++;
                                in_border_vertical=1;
                        }


                }
                else{
                        in_border_vertical=0;
                }
	}
	printf("Bordures horizontales détectées : %d\n",
       horizontal_borders);
	grid->columns = vertical_borders - 1;
	if (grid->rows <= 0 || grid->columns <= 0)
    	{
        	return 0;
    	}

     /* Calculation of the theoretical dimensions of the cells */
    	grid->cell_width = (right - left) / grid->columns;
    	grid->cell_height = (bottom - top) / grid->rows;

    	if (grid->cell_width <= 0 || grid->cell_height <= 0)
    	{
        	return 0;
    	}

    	return 1;

}

