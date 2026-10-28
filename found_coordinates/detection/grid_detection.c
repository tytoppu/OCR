#include "grid_detection.h"

#include <stddef.h>


//
//


//verifie si pixel noir
//<128 sinon blanc
static int is_black(const unsigned char *image,int width,int height,int stride,int x, int y)
{
    	(void)width;

    	if (x < 0 || y < 0 || x >= width || y >= height)
        	return 0;

    	return image[(size_t)y * (size_t)stride + (size_t)x] < 128;
}



//chercher la plus longue ligne
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





//chercher la plus longue colone
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

//cherche coordonné de la grille

//stride->nb octet entre le début de 2 ligne cons
int detect_grid_bounds(const unsigned char *image,
                       int width,
                       int height,
                       int stride,
                       GridCoordinates *grid)
{
    int x;//colonnes
    int y;//lignes
    int min_run;//min suite noire
    int top = -1;//ligne lim sup
    int bottom = -1;//ligne lim inf
    int left = -1;//col lim sup
		 
    int right = -1;//col lim inf
    int best_horizontal_run = 0;//plus grande suite
    int best_vertical_run = 0;
    int horizontal_threshold;
    int vertical_threshold;

    if (image == NULL || grid == NULL || width <= 0 || height <= 0 ||
        stride < width)
        return 0;

//longueur minimal pixel noir estimation    
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





