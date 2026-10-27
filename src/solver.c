#include "../include/solver.h"

char **gridify(const char *filename, size_t *rows_out) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;
    size_t cap = 16, rows = 0;
    char **grid = malloc(cap * sizeof *grid);
    if (!grid) { fclose(f); return NULL; }
    char buf[1024];
    while (fgets(buf, sizeof buf, f)){
        buf[strcspn(buf, "\r\n")] = '\0';
        if (rows == cap){
            cap *= 2;
            char **tmp = realloc(grid, cap * sizeof *grid);
            if(!tmp){
                for (size_t i = 0; i < rows; i++) free(grid[i]);
                    free(grid);
                fclose(f);
                return NULL;
            }
            grid = tmp;
        }
        grid[rows] = strdup(buf);
        if (!grid[rows]){
            for (size_t i = 0; i < rows; i++) free(grid[i]);
                free(grid);
            fclose(f);
            return NULL;
        }
        rows++;
    }
    fclose(f);
    grid[rows] = NULL; 
    *rows_out = rows;
    return grid;
}

void solver(char **grid, int coord[4], char *word) {
    if (!grid || !word) return;
    size_t len = strlen(word);
    if (len == 0) return;
    size_t nrows = 0;
    while (grid[nrows]) nrows++;
    static const int dr[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
    static const int dc[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
    for (size_t r = 0; r < nrows; r++) {
        size_t cols = strlen(grid[r]);
        for (size_t c = 0; c < cols; c++) {
            if (tolower((unsigned char)grid[r][c])
             != tolower((unsigned char)word[0])) continue;;
            for (int d = 0; d < 8; d++) {
                size_t k;
                long cr = (long)r, cc = (long)c;
                for (k = 0; k < len; k++) {
                    if (cr < 0 || cr >= (long)nrows) break;
                    if (cc < 0 || cc >= (long)strlen(grid[cr])) break;
                    if (tolower((unsigned char)grid[cr][cc])
                     != tolower((unsigned char)word[k])) break;
                    if (k + 1 < len) { cr += dr[d]; cc += dc[d]; }
                }
                if (k == len) {
                    coord[0] = (int)r;  coord[1] = (int)c;
                    coord[2] = (int)cr; coord[3] = (int)cc;
                    return;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) { printf("Not Found\n"); return 0; }
    int coord[4] = {-1, -1, -1, -1};
    size_t rows = 0;
    char **grid = gridify(argv[1], &rows);
    if (!grid) { printf("Not Found\n"); return 0; }
    solver(grid, coord, argv[2]);
    if (coord[3] == -1)
        printf("Not Found\n");
    else
        printf("(%d,%d)(%d,%d)\n", coord[0], coord[1], coord[2], coord[3]);
    for (size_t i = 0; i < rows; i++) free(grid[i]);
    free(grid);
    return 0;
}