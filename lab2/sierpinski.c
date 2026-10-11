#include <stdio.h>
#include <stdlib.h>


int nb_columns() { 
    return 129; 
}
int nb_lines() { 
    return 65; 
}

void grid_init(char grid[], char pixel) {
    int total_elements = nb_lines() * nb_columns();
    for (int i = 0; i < total_elements; i++) {
        grid[i] = pixel;
    }
}

void grid_display(char grid[]) {
    int cols = nb_columns();
    for (int y = 0; y < nb_lines(); y++) {
        for (int x = 0; x < cols; x++) {
            printf("%c", grid[y * cols + x]);
        }
        printf("\n");
    }
}

void plot_point(char grid[], int x, int y, char pixel) {
    if (x < 0 || x >= nb_columns() || y < 0 || y >= nb_lines()) {
        printf("Error: Coordinates (%d, %d) are out of bounds.\n", x, y);
        exit(EXIT_FAILURE);
    }
    int row = nb_lines() - y - 1;
    grid[row * nb_columns() + x] = pixel;
}


int sort_get_index(float tab[], int top, float val) {
    for (int i = 0; i < top; i++) {
        if (tab[i] > val) {
            return i;
        }
    }
    return top;
}

void sort_insert_at(float tab[], int i, int top, float val) {
    for (int j = top; j > i; j--) {
        tab[j] = tab[j - 1];
    }
    tab[i] = val;
}

void sort_insert(float tab[], int top, float val) {
    int i = sort_get_index(tab, top, val);
    sort_insert_at(tab, i, top, val);
}


void plot_vline(char grid[], int x, float fy0, float fy1, char pixel) {
    int y0 = (int)(fy0 + 0.5);
    int y1 = (int)(fy1 + 0.5);

    if (fy0 == (float)y0 - 0.5) {
        y0--;
    }

    for (int y = y0; y <= y1; y++) {
        plot_point(grid, x, y, pixel);
    }
}


struct point {
    float x;
    float y;
};

void plot_poly_sweep(char grid[], struct point p[], int n, int x, char pixel) {
    float vlines[n * 4];
    int top = 0;

    for (int i = 0; i < n; i++) {
        int j = (i == 0) ? n - 1 : i - 1;

        if ((int)p[i].x == x) {
            if ((int)p[j].x == x) {
                float min_y = p[i].y < p[j].y ? p[i].y : p[j].y;
                float max_y = p[i].y > p[j].y ? p[i].y : p[j].y;
                plot_vline(grid, x, min_y, max_y, pixel);
            } else {
                plot_vline(grid, x, p[i].y, p[i].y, pixel);
            }
        }

        if (p[i].x <= p[j].x) {
            if (p[i].x <= x && x < p[j].x) {
                float inter = p[i].y + (x - p[i].x) * (p[j].y - p[i].y) / (p[j].x - p[i].x);
                sort_insert(vlines, top, inter);
                top++;
            }
        } else {
            if (p[j].x <= x && x < p[i].x) {
                float inter = p[i].y + (x - p[i].x) * (p[j].y - p[i].y) / (p[j].x - p[i].x);
                sort_insert(vlines, top, inter);
                top++;
            }
        }
    }

    for (int k = 0; k < top; k += 2) {
        if (k + 1 < top) {
            plot_vline(grid, x, vlines[k], vlines[k + 1], pixel);
        }
    }
}

void plot_poly(char grid[], struct point p[], int n, char pixel) {
    for (int x = 0; x < nb_columns(); x++) {
        plot_poly_sweep(grid, p, n, x, pixel);
    }
}


void plot_triangle(char grid[], struct point p1, struct point p2, struct point p3, char pixel) {
    struct point p[3] = {p1, p2, p3};
    plot_poly(grid, p, 3, pixel);
}

struct point line_middle(struct point p1, struct point p2) {
    struct point mid;
    mid.x = (p1.x + p2.x) / 2.0;
    mid.y = (p1.y + p2.y) / 2.0;
    return mid;
}

void sierpinski(char grid[], struct point p1, struct point p2, struct point p3, int n, char pixel) {
    if (n == 1) {
        plot_triangle(grid, p1, p2, p3, pixel);
    } else {
        struct point m12 = line_middle(p1, p2);
        struct point m23 = line_middle(p2, p3);
        struct point m31 = line_middle(p3, p1);

        sierpinski(grid, p1, m12, m31, n - 1, pixel);
        sierpinski(grid, m12, p2, m23, n - 1, pixel);
        sierpinski(grid, m31, m23, p3, n - 1, pixel);
    }
}


int main(int argc, char* argv[]) {
    int size = nb_lines() * nb_columns();
    char grid[size]; 
    
    grid_init(grid, ' ');

    struct point p1 = {0, 0};
    struct point p2 = {64, 64};
    struct point p3 = {128, 0};

    sierpinski(grid, p1, p2, p3, 6, '*');

    grid_display(grid);
    
    return 0;
}
