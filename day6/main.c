#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFF_CAP 1024
#define FISH_CAP 1024

#define DAYS_AMOUNT 9


void print_days(long* days) {
    for (int i = 0; i < DAYS_AMOUNT; ++i) {
        printf("Day %d: %ld\n", i, days[i]);
    }
    printf("\n");
}

void simulate_day(long* days) {
    long back_buffer[DAYS_AMOUNT] = {0};
    back_buffer[8] += days[0];
    back_buffer[6] += days[0];

    for (int i = 1; i < DAYS_AMOUNT; ++i) {
        back_buffer[i - 1] += days[i];
    }

    memcpy(days, back_buffer, sizeof(long) * DAYS_AMOUNT);
}

long simulate_fishes(int* fishes, int size, int days_to_simulate) {
    long days[DAYS_AMOUNT] = {0};

    for (int i = 0; i < size; ++i) {
        days[fishes[i]]++;
    }

    for (int i = 0; i < days_to_simulate; ++i) {
        simulate_day(days);
    }

    long result = 0;
    for (int i = 0; i < DAYS_AMOUNT; ++i) {
        result += days[i];
    }

    return result;
}

long part1(int* fishes, int size) {
    return simulate_fishes(fishes, size, 80);
}

long part2(int* fishes, int size) {
    return simulate_fishes(fishes, size, 256);
}

int main(void) {
    // devide the amount of days subtract day by six, you get amount of children
    const char* file_name = "./input.txt";
    FILE* f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'\n", file_name);
        exit(1);
    }

    char buffer[BUFF_CAP];
    int fishes[FISH_CAP] = {0};
    int fish_amount = 0;

    while (fgets(buffer, BUFF_CAP, f)) {
        char* start = buffer;
        char* end;

        while (start) {
            int num = (int) strtol(start, &end, 10);
            fishes[fish_amount++] = num;
            if (end == NULL || *end == '\0' || *end == '\n') {
                break;
            }

            start = end + 1;
        }
    }

    fclose(f);

    printf("part1:\t%ld\n", part1(fishes, fish_amount));
    printf("part2:\t%ld\n", part2(fishes, fish_amount));

    return 0;
}
