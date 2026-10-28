#include "../detection/grid_detection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_token(FILE *file, char *buffer, size_t capacity)
{
    int c;
    size_t i = 0;

    do
    {
        c = fgetc(file);
        if (c == '#')
        {
            while (c != '\n' && c != EOF)
                c = fgetc(file);
        }
    } while (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '#');

    if (c == EOF)
        return 0;

    do
    {
        if (i + 1 < capacity)
            buffer[i++] = (char)c;
        c = fgetc(file);
    } while (c != EOF && c != ' ' && c != '\n' && c != '\r' && c != '\t');

    buffer[i] = '\0';
    return 1;
}





int main(int argc, char **argv)
{
    FILE *file;
    char token[64];
    int width, height, max_value;
    unsigned char *pixels;
    size_t pixel_count, bytes_read;
    GridCoordinates grid;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s image.pgm\n", argv[0]);
        return EXIT_FAILURE;
    }

    file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        perror("Could not open image");
        return EXIT_FAILURE;
    }

    if (!read_token(file, token, sizeof(token)) || strcmp(token, "P5") != 0)
    {
        fprintf(stderr, "Error: expected a binary PGM (P5) image.\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    if (!read_token(file, token, sizeof(token)))
    {
        fprintf(stderr, "Error: missing image width.\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    width = atoi(token);

    if (!read_token(file, token, sizeof(token)))
    {
        fprintf(stderr, "Error: missing image height.\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    height = atoi(token);

    if (!read_token(file, token, sizeof(token)))
    {
        fprintf(stderr, "Error: missing maximum pixel value.\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    max_value = atoi(token);

    if (width <= 0 || height <= 0 || max_value != 255)
    {
        fprintf(stderr, "Error: dimensions must be positive and max value 255.\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    pixel_count = (size_t)width * (size_t)height;
    pixels = malloc(pixel_count);
    if (pixels == NULL)
    {
        fprintf(stderr, "Error: not enough memory.\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    bytes_read = fread(pixels, 1, pixel_count, file);
    fclose(file);

    if (bytes_read != pixel_count)
    {
        fprintf(stderr, "Error: incomplete pixel data in PGM file.\n");
        free(pixels);
        return EXIT_FAILURE;
    }

    if (!detect_grid_bounds(pixels, width, height, width, &grid))
    {
        printf("Grid detection failed.\n");
        free(pixels);
        return EXIT_FAILURE;
    }

    printf("Grid detected (estimated coordinates):\n");
    printf("Top-left:     (%d, %d)\n",
           grid.bounds.top_left.x, grid.bounds.top_left.y);
    printf("Top-right:    (%d, %d)\n",
           grid.bounds.top_right.x, grid.bounds.top_right.y);
    printf("Bottom-right: (%d, %d)\n",
           grid.bounds.bottom_right.x, grid.bounds.bottom_right.y);
    printf("Bottom-left:  (%d, %d)\n",
           grid.bounds.bottom_left.x, grid.bounds.bottom_left.y);
	detect_grid_dimensions(pixels, width, height, width, &grid);
	printf("Nombre de lignes : %d\n", grid.rows);
printf("Nombre de colonnes : %d\n", grid.columns);
printf("Largeur d'une cellule : %d\n", grid.cell_width);
printf("Hauteur d'une cellule : %d\n", grid.cell_height);
    free(pixels);
    return EXIT_SUCCESS;
}

