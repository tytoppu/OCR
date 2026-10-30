#include "../../../include/new_grid_list_detection.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>




//checks if one pixel is colors //<128 otherwise white

int is_colors(struct simple_surface* simple,int x, int y,int colors)    {                                                                               /*                                                                              return 1 if its colors, 0 if its not                            */                                                                                                                                              if (!simple || x < 0 || y < 0 || x >= simple->w || y >= simple->h)                                                                                      errx(EXIT_FAILURE, "is_colors : wrong input");                                                                                          return simple->gray[(size_t)y * (size_t)simple->w + (size_t)x] == colors;
}                                                                        
/*
static void set_rectangle(Rectangle *r,
                          int x_min,
                          int y_min,
                          int x_max,
                          int y_max)
{
    r->top_left.x = x_min;
    r->top_left.y = y_min;

    r->top_right.x = x_max;
    r->top_right.y = y_min;

    r->bottom_right.x = x_max;
    r->bottom_right.y = y_max;

    r->bottom_left.x = x_min;
    r->bottom_left.y = y_max;
}
*/

void dectet_grid_and_list(struct simple_surface* simple, struct simple_surface* grid, struct simple_surface* list, int colors)
{
	if (!simple || !grid || !list)
		errx(EXIT_FAILURE, "dectet_grid_and_list : wrong input");
	int width[simple->w];
	int height[simple->h];
	int w_count=0;
	int h_count=0;
	for (int w=0; w<simple->w; w++)
	{
		w_count=0;
		for (int h=0; h<simple->h; h++)
		{
			if (is_colors(simple, w, h, colors))
				w_count++;
		}
		width[w]=w_count;
	}
	for (int h=0; h<simple->h; h++)
        {
                h_count=0;
                for (int w=0; w<simple->w; w++)
                {
                        if (is_colors(simple, w, h, colors))
                                h_count++;
                }
                height[h]=h_count;
        }
	int t_l_x1=0;
	int t_l_y1=0;

	int t_r_x1=0;
	int t_r_y1=0;

	int b_l_x1=0;
	int b_l_y1=0;

	int b_r_x1=0;
	int b_r_y1=0;

	//2 
	//top
	int t_l_x2=0;
        int t_l_y2=0;

        int t_r_x2=0;
        int t_r_y2=0;

	//bottom
        int b_l_x2=0;
        int b_l_y2=0;

        int b_r_x2=0;
        int b_r_y2=0;

	

	int left = 0;
	int right = simple->w - 1;
	int top = 0;
	int bottom = simple->h - 1;

	/* Find the first non-empty column */
	while (left < simple->w && width[left] == 0)
    		left++;

	/* Find the last non-empty column */
	while (right >= 0 && width[right] == 0)
   		right--;

	/* Find the first non-empty row */
	while (top < simple->h && height[top] == 0)
    		top++;

	/* Find the last non-empty row */
	while (bottom >= 0 && height[bottom] == 0)
    		bottom--;
	

	int w_start = -1;
	int w_l_max = 0;
	int w_start_max = -1;
	//start_max+longueur_max-1 -> last empty
	for (int w = left; w <= right; w++)
	{
    		if (width[w] == 0)
    		{
        		if (w_start == -1)
            		w_start = w;
    		}
    		else
    		{
        		if (w_start != -1)
        		{
            			int l = w - w_start;

            			if (l > w_l_max)
            			{
                			w_l_max = l;
                			w_start_max = w_start;
            			}

            			w_start = -1;
        		}
    		}
	}

	if (w_start != -1)
	{
    		int l = right - w_start+1;

    		if (l > w_l_max)
    		{
        		w_l_max = l;
        		w_start_max = w_start;
    		}
	}


	int h_start = -1;
	int h_l_max = 0;
	int h_start_max = -1;

	for (int h = top; h <= bottom; h++)
	{
        	if (height[h] == 0)
        	{
                	if (h_start == -1)
                        	h_start = h;
        	}
        	else
        	{
                	if (h_start != -1)
                	{
                        	int l = h - h_start;

                        	if (l > h_l_max)
                        	{
                                	h_l_max = l;
                                	h_start_max = h_start;
                        	}

                        	h_start = -1;
                	}
        	}
	}

	if (h_start != -1)
	{
        	int l = bottom - h_start + 1;

 	      	if (l > h_l_max)
        	{
                	h_l_max = l;
                	h_start_max = h_start;
        	}
	}



	if (w_l_max > 0&& (h_l_max == 0
        	|| (double)w_l_max / (right - left + 1)
           	>= (double)h_l_max / (bottom - top + 1)))
	{

        	t_l_x1 = left;
        	t_l_y1 = top;

        	t_r_x1 = w_start_max - 1;
        	t_r_y1 = top;

        	b_l_x1 = left;
        	b_l_y1 = bottom;

        	b_r_x1 = w_start_max - 1;
        	b_r_y1 = bottom;


        	// Region 2
        	t_l_x2 = w_start_max + w_l_max;
        	t_l_y2 = top;

        	t_r_x2 = right;
        	t_r_y2 = top;

        	b_l_x2 = w_start_max + w_l_max;
        	b_l_y2 = bottom;

        	b_r_x2 = right;
        	b_r_y2 = bottom;
	}
	else if (h_l_max > 0)
	{

        	// Region 1
        	t_l_x1 = left;
        	t_l_y1 = top;

        	t_r_x1 = right;
        	t_r_y1 = top;

        	b_l_x1 = left;
        	b_l_y1 = h_start_max - 1;

        	b_r_x1 = right;
        	b_r_y1 = h_start_max - 1;


        	// Region 2
        	t_l_x2 = left;
        	t_l_y2 = h_start_max + h_l_max;

        	t_r_x2 = right;
        	t_r_y2 = h_start_max + h_l_max;

        	b_l_x2 = left;
        	b_l_y2 = bottom;

        	b_r_x2 = right;
        	b_r_y2 = bottom;
	}
	else
	{
        	errx(EXIT_FAILURE, "dectet_grid_and_list : No suitable separating gap found");
	}

	/* Calculate the area of both regions */
	int area1 = (t_r_x1 - t_l_x1 + 1)
          	* (b_l_y1 - t_l_y1 + 1);

	int area2 = (t_r_x2 - t_l_x2 + 1)
          	* (b_l_y2 - t_l_y2 + 1);

	if (area2 > area1)
	{
        	int temp;

        	temp = t_l_x1;
        	t_l_x1 = t_l_x2;
        	t_l_x2 = temp;

        	temp = t_l_y1;
        	t_l_y1 = t_l_y2;
        	t_l_y2 = temp;

        	temp = t_r_x1;
        	t_r_x1 = t_r_x2;
        	t_r_x2 = temp;

        	temp = t_r_y1;
       		t_r_y1 = t_r_y2;
        	t_r_y2 = temp;

        	temp = b_l_x1;
        	b_l_x1 = b_l_x2;
        	b_l_x2 = temp;

       		temp = b_l_y1;
        	b_l_y1 = b_l_y2;
        	b_l_y2 = temp;

        	temp = b_r_x1;
        	b_r_x1 = b_r_x2;
        	b_r_x2 = temp;

        	temp = b_r_y1;
        	b_r_y1 = b_r_y2;
        	b_r_y2 = temp;
	}

	grid->w=t_r_x1 - t_l_x1 + 1;
	grid->h=b_l_y1 - t_l_y1 + 1;


	list->w=t_r_x2 - t_l_x2 + 1;
	list->h=b_l_y2 - t_l_y2 + 1;

	make_chunck(grid,t_l_x1,t_l_y1,b_r_x1,b_r_y1);
	make_chunck(list,t_l_x2,t_l_y2,b_r_x2,b_r_y2);
}

