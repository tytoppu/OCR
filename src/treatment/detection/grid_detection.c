#include "grid_detection.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>


//
//
//checks if one pixel is colors //<128 otherwise white

int is_colors(struct simple_surface* simple,int x, int y,int colors)
{
	/*
		return 1 if its colors, 0 if its not
	*/

    	if (!simple || x < 0 || y < 0 || x >= simple->w || y >= simple->h)
        	errx(EXIT_FAILURE, "is_colors : wrong input");

    	return simple->gray[(size_t)y * (size_t)simple->w + (size_t)x] == colors;
}


//search for the longest row
int longest_black_run_in_row(struct simple_surface* simple, int y, int colors)
{
	if (!simple) errx(EXIT_FAILURE, "longest_black_run_in_row : wrong input");
	int x;
    int current_run = 0;
    int longest_run = 0;
    	for (x = 0; x < simple->w; x++)
   	{
       	if (is_colors(simple, x, y, colors))
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

int longest_black_run_in_column(struct simple_surface* simple, int x, int colors)
{
	if (!simple) errx(EXIT_FAILURE, "longest_black_run_in_column : wrong input");
   	int y;
   	int current_run = 0;
   	int longest_run = 0;
    	for (y = 0; y < simple->h; y++)
   	{
       	if (is_colors(simple, x, y, colors))
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

//stride -> number of bytes between the start of 2 consecutive lines : pitch
void detect_grid_bounds(struct simple_surface* simple,
                       GridCoordinates *grid, int colors)
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

    if (!simple)
        errx(EXIT_FAILURE, "detect_grid_bounds : wrong input");
//minimal black pixel length estimate
    min_run = (simple->w < simple->h ? simple->w : simple->h) / 12;
    if (min_run < 10)
        min_run = 10;

    horizontal_threshold = min_run;
    vertical_threshold = min_run;

    for (y = 0; y < simple->h; y++)
    {
        int run = longest_black_run_in_row(simple, y,colors);

        if (run >= horizontal_threshold)
        {
            		if (top == -1)
                		top = y;
            		bottom = y;

            		if (run > best_horizontal_run)
                		best_horizontal_run = run;
        	}
    	}

    	for (x = 0; x < simple->w; x++)
    	{
        	int run = longest_black_run_in_column(simple, x,colors);

        	if (run >= vertical_threshold)
        	{
            		if (left == -1)
                		left = x;
            		right = x;

            		if (run > best_vertical_run)
                		best_vertical_run = run;
        	}
    	}

    	if (top < 0 || bottom <= top || left < 0 || right <= left || bottom - top < min_run || right - left < min_run)
        	errx(EXIT_FAILURE, "detect_grid_bounds : wrong result");

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

}





void detect_grid_dimensions(struct simple_surface* simple,
							GridCoordinates *grid,
						   	int colors)
{
	if (!simple)
		errx(EXIT_FAILURE, "detect_grid_dimensions : wrong input");

	int left = grid->bounds.top_left.x;
	int right = grid->bounds.top_right.x;
	int top = grid->bounds.top_left.y;
	int bottom = grid->bounds.bottom_left.y;
	int grid_width = right - left + 1;
	int black_pixels = 0;

	if (left < 0 || right >= simple->w ||
    		top < 0 || bottom >= simple->h ||
    		right <= left || bottom <= top)
		errx(EXIT_FAILURE, "detect_grid_dimensions : wrong value in grid");
	
	double black_ratio;
	int horizontal_borders = 0;//counts number of edges detected
	int in_border = 0; //indicates if already in a border
	for (int y=top; y<=bottom; y++)
	{
		black_pixels=0;
		for (int x = left; x <= right; x++)
		{
    			if (simple->gray[y * simple->w + x] ==colors)
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
			if (simple->gray[y*simple->w+x]==colors)
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
        	errx(EXIT_FAILURE, "detect_grid_dimensions : wrong result 1");
    	}

     /* Calculation of the theoretical dimensions of the cells */
    	grid->cell_width = (right - left) / grid->columns;
    	grid->cell_height = (bottom - top) / grid->rows;

    	if (grid->cell_width <= 0 || grid->cell_height <= 0)
    	{
        	errx(EXIT_FAILURE, "detect_grid_dimensions : wrong result 2");
    	}

}

