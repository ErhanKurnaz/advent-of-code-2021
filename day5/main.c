#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define BUFF_CAP 256
#define LINES_CAP 1024

#define TEST 0

#if TEST
#define GRID_SIZE 10
#else
#define GRID_SIZE 1000
#endif

#define POINT_FMT "(x: %d, y: %d)"
#define LINE_FMT "start: "POINT_FMT" -> end: "POINT_FMT

#define PRINT_POINT(point) point.x, point.y
#define PRINT_LINE(line) PRINT_POINT(line.start), PRINT_POINT(line.end)

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

struct {
    int x;
    int y;
} typedef Point;


struct {
    Point start;
    Point end;
} typedef Line;


void print_grid(int grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}

void print_lines(Line* lines, int line_count) {
    for (int i = 0; i < line_count; ++i) {
        printf(LINE_FMT"\n", PRINT_LINE(lines[i]));
    }
    printf("\n");
}


int part1(Line* lines, int line_count) {
    int result = 0;
    int grid[GRID_SIZE][GRID_SIZE] = {0};

    for (int i = 0; i < line_count; ++i) {
        Line line = lines[i];
        if (line.start.x == line.end.x || line.start.y == line.end.y) {
            int x = line.start.x;
            int y = line.start.y;

            int endX = line.end.x;
            int endY = line.end.y;
            // if the delta is positive that means that
            // the left hand side is further away than the right hand side
            // so we need to decrease to reach our destination
            int delta = x - endX + y - endY;
            if (x != endX) {
                if (delta < 0) {
                    endX++;
                } else {
                    endX--;
                }
            }
            if (y != endY) {
                if (delta < 0) {
                    endY++;
                } else {
                    endY--;
                }
            }

            while (x != endX || y != endY) {
                grid[y][x]++;

                if (grid[y][x] == 2) {
                    result++;
                }
                if (x < endX) {
                    x++;
                } else if (x > endX) {
                    x--;
                }

                if (y < endY) {
                    y++;
                } else if (y > endY) {
                    y--;
                }
            }
        }
    }

    // print_grid(grid);

    return result;
}

int find_angle(Point p1, Point p2) {
    return (int) (atan2(p1.y - p2.y, p1.x - p2.x) * 180.0f / 3.14f);
}

int part2(Line* lines, int line_count) {
    int result = 0;
    int grid[GRID_SIZE][GRID_SIZE] = {0};

    for (int i = 0; i < line_count; ++i) {
        Line line = lines[i];
        int angle = abs(find_angle(line.start, line.end));
        if (line.start.x == line.end.x || line.start.y == line.end.y || angle == 45 || angle == 135) {
            int x = line.start.x;
            int y = line.start.y;

            int endX = line.end.x;
            int endY = line.end.y;
            // if the delta is positive that means that
            // the left hand side is further away than the right hand side
            // so we need to decrease to reach our destination
            int delta = x - endX + y - endY;
            if (x != endX) {
                if ((delta < 0) || (delta == 0 && x < endX)) {
                    endX++;
                } else {
                    endX--;
                }
            }
            if (y != endY) {
                if ((delta < 0) || (delta == 0 && y < endY)) {
                    endY++;
                } else {
                    endY--;
                }
            }

            while (x != endX || y != endY) {
                grid[y][x]++;

                if (grid[y][x] == 2) {
                    result++;
                }
                if (x < endX) {
                    x++;
                } else if (x > endX) {
                    x--;
                }

                if (y < endY) {
                    y++;
                } else if (y > endY) {
                    y--;
                }
            }
        }
    }

    return result;
}

int main(void) {
    #if TEST
    const char* file_name = "./input.test.txt";
    #else
    const char* file_name = "./input.txt";
    #endif
    FILE* f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'\n", file_name);
        exit(1);
    }

    char buffer[BUFF_CAP];
    Line lines[LINES_CAP];
    int line_amount = 0;

    while (fgets(buffer, BUFF_CAP, f)) {
        char* start = buffer;

        bool skippedArrow = false;
        Point startPoint;
        Point endPoint;
        while (start) {
            char* end;
            int num1 = (int) strtol(start, &end, 10);
            int num2 = (int) strtol(end + 1, &end, 10);
            Point point = {.x = num1, .y = num2};

            if (skippedArrow) {
                endPoint = point;
                Line line;
                line.start = startPoint;
                line.end = endPoint;
                lines[line_amount++] = line;
                start = end;
            } else {
                startPoint = point;
                start = end + 3;
                skippedArrow = true;
            }

            if (end == NULL || *end == '\0' || *end == '\n') {
                break;
            }
        }
    }

    fclose(f);

    printf("part1:\t%d\n", part1(lines, line_amount));
    printf("part2:\t%d\n", part2(lines, line_amount));

    return 0;
}
