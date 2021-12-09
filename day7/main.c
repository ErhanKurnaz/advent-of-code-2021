#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define BUFF_CAP 4096
#define NUMS_CAP 4096

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

// To dumb to do merge sort, so here is insertion sort
void insertion_sort(int* arr, int size) {
    for (int i = 0; i < size; ++i) {
        int element = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > element) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = element;
    }
}

int part1(int* positions, int size) {
    int middel_val = positions[size / 2];

    int result = 0;

    for (int i = 0; i < size; ++i) {
        result += abs(positions[i] - middel_val);
    }

    return result;
}

// to stupid to think about a math solution for this one
int part2(int* positions, int size) {
    int result = INT_MAX;

    for (int i = 0; i < size; ++i) {
        int total = 0;
        for (int j = 0; j < size; ++j) {
            int distance = abs(i - positions[j]);
            int fuel = (distance * (distance + 1)) / 2;
            total += fuel;
        }

        result = MIN(result, total);
    }

    return result;
}

void print_positions(int* positions, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%d, ", positions[i]);
    }

    printf("\n");
}

int main(void) {
    const char* file_name = "./input.txt";

    FILE *f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'\n", file_name);
    }

    char buffer[BUFF_CAP];

    int positions[NUMS_CAP] = {0};
    int size = 0;
    while(fgets(buffer, BUFF_CAP, f)) {
        char* start = buffer;
        char* end;

        while (start) {
            int num = (int) strtol(start, &end, 10);
            positions[size++] = num;
            if (end == NULL || *end == '\0' || *end == '\n') {
                break;
            }

            start = end + 1;
        }
    }

    fclose(f);
    int unsorted_positions[NUMS_CAP] = {0};
    memcpy(unsorted_positions, positions, sizeof(int) * NUMS_CAP);
    insertion_sort(positions, size);
    printf("%f\n", round((double) (size + 1) / 2));

    printf("part1:\t%d\n", part1(positions, size));
    printf("part2:\t%d\n", part2(positions, size));
    return 0;
}
