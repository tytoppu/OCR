#include "new_grid_list_detection.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>


void where_is_anchor(struct simple_surface* simple, int* col, int* row, Uint8 anchor){
    /*
     * fills col and row to map where the anchor is on simple
     */
    for(int y = 0; y<simple->h; y++){
        for(int x = 0; x<simple->w; x++){
            if (simple->gray[y*simple->w + x] == anchor) {
                col[x]++;
                row[y]++;
            }
        }
    }
	//for(int i = 0; i<simple->w; i++) printf("%i\n",col[i]);
}


struct simple_surface* scrap(struct simple_surface* simple, int* col, int* row, 
        int* l, int* r, int* t, int* b){
    /*
     * crops the image to the minimum (basically removes the margins)
     * returns the left right top bottom of this new cropped image
     */
    int left = 0;
    int right = simple->w - 1;
    int top = 0;
    int bottom = simple->h - 1;
    while (left < simple->w && col[left] < simple->h*0.01) left++;
    while (right > 0 && col[right] < simple->h*0.01) right--;
    while (top < simple->h && row[top] < simple->w*0.01) top++;
    while (bottom > 0 && row[bottom] < simple->w*0.01) bottom--;

    if (left > right || top > bottom)
        errx(EXIT_FAILURE, "scrap : no ink in this region");

    *l = left;
    *r = right;
    *t = top;
    *b = bottom;

    return make_chunk(simple, left, top, right, bottom);
}


struct simple_surface* mini_scrap(struct simple_surface* simple, int x1, int y1, int x2, int y2, Uint8 anchor){
    struct simple_surface* temp = make_chunk(simple, x1, y1, x2, y2);
    int left;
    int right;
    int top;
    int bottom;
    int *col = calloc(temp->w, sizeof(int));
    int *row = calloc(temp->h, sizeof(int));
    if (!col || !row) errx(EXIT_FAILURE, "mini_scrap : calloc");
    where_is_anchor(temp, col, row, anchor);
    struct simple_surface* mini_scratch = scrap(temp, col, row, &left, &right, &top, &bottom);
    free(col);
    free(row);
    destroy_simple_surface(temp);
    return mini_scratch;
}


void max_empty_range (int* array, int size, double threshold, int* len_out, int* x_out){
    int len = 0;
    int max_len = 0;
    int x = 0;
    int max_x = 0;
    int flag = 0;                   // 1 when in accumulation mode and 0 otherwise
    for(int i = 0; i < size; i++){
        if (array[i] < threshold){
            if (flag == 0){
                x = i;
                len = 0;
                flag = 1;
            }
            len++;
        }
        else {
            if (flag == 1){
                if (len > max_len){
                    max_len = len;
                    max_x = x;
                }
                flag = 0;
            }

        }
    }
    if (flag == 1 && len > max_len){ 
        max_len = len; 
        max_x = x; 
    }
    *len_out = max_len;
    *x_out = max_x;
}



void detect_grid_and_list(struct simple_surface* simple, struct simple_surface** grid, struct simple_surface** list, Uint8 anchor)
{

    int *col = calloc(simple->w, sizeof(int));
    int *row = calloc(simple->h, sizeof(int));
    if (!col || !row) errx(EXIT_FAILURE, "detect_grid_and_list : calloc");
    where_is_anchor(simple, col, row, anchor);
	//for(int i = 0; i<simple->w; i++) printf("%i\n",col[i]);

    int left;
    int right;
    int top;
    int bottom;

    struct simple_surface* scratch = scrap(simple, col, row, &left, &right, &top, &bottom);
    int len_col;
    int x_col;
    int len_row;
    int y_row;
    max_empty_range (col + left, right - left + 1, (double) scratch->h * 0.01, &len_col, &x_col);
    max_empty_range (row + top, bottom - top + 1, (double) scratch->w * 0.01, &len_row, &y_row);

    double A = (double) len_col / (right - left + 1);
    double B = (double) len_row / (bottom - top + 1);

    if (A < 0.03 && B < 0.03)
        errx(EXIT_FAILURE, "detect_grid_and_list : no clear gutter");


    struct simple_surface* first = NULL;
    struct simple_surface* second = NULL;

    if (A>=B){                               // cut along the height (so we work on x, on the width)
        first = make_chunk(scratch, 0, 0, x_col-1, scratch->h-1);
        second = make_chunk(scratch, x_col+len_col, 0, scratch->w-1, scratch->h-1);
    }

    else{
        first = make_chunk(scratch, 0, 0, scratch->w-1, y_row-1);
        second = make_chunk(scratch, 0, y_row+len_row, scratch->w-1, scratch->h-1);
    }

    free(col);
    free(row);
    int *first_col = calloc(first->w, sizeof(int));
    int *first_row = calloc(first->h, sizeof(int));
    int *second_col = calloc(second->w, sizeof(int));
    int *second_row = calloc(second->h, sizeof(int));
    if (!first_col || !first_row || !second_col || !second_row)
        errx(EXIT_FAILURE, "detect_grid_and_list : calloc");
    where_is_anchor(first, first_col, first_row, anchor);
    where_is_anchor(second, second_col, second_row, anchor);
    struct simple_surface* scratch_first = scrap(first, first_col, first_row, &left, &right, &top, &bottom);
    struct simple_surface* scratch_second = scrap(second, second_col, second_row, &left, &right, &top, &bottom);

    long first_area = (long) scratch_first->w * scratch_first->h;
    long second_area = (long) scratch_second->w * scratch_second->h;

    if (first_area>=second_area){
        *grid = scratch_first;
        *list = scratch_second;
    }
    else {
        *grid = scratch_second;
        *list = scratch_first;
    }
    free(first_col);
    free(first_row);
    free(second_col);
    free(second_row);
    destroy_simple_surface(scratch);
    destroy_simple_surface(first);
    destroy_simple_surface(second);
}

