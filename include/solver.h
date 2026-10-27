#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>


char **gridify(const char *filename, size_t *rows_out);
void solver(char **grid, int coord[4], char *word);


#endif