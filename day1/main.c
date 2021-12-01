#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define BUFF_CAP 256
#define NUM_CAP 2048

int part1(long *numbers, int length) {
    assert(length > 1 && "not enough numbers in array");

    long last_num = numbers[0];
    int result = 0;
    for (int i = 1; i < length; ++i) {
        if (numbers[i] > last_num) {
            result++;
        }

        last_num = numbers[i];
    }

    return result;
}

int part2(long *numbers, int length) {
    assert(length > 2 && "not enough numbers in array");

    long last_num = numbers[0] + numbers[1] + numbers[2];
    int result = 0;
    for (int i = 1; i < length - 2; ++i) {
        long new_res = numbers[i] + numbers[i + 1] + numbers[i + 2];
        if (new_res > last_num) {
            result++;
        }

        last_num = new_res;
    }

    return result;
}

int main(void) {
    FILE *f = fopen("./input.txt", "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file 'input.txt'");
        exit(1);
    }

    char buffer[BUFF_CAP];
    long numbers[NUM_CAP];
    int read = 0;
    while (fgets(buffer, BUFF_CAP, f)) {
        numbers[read++] = strtol(buffer, (char **)NULL, 10);
    }

    fclose(f);

    printf("part1:\t%d\n", part1(numbers, read));
    printf("part2:\t%d\n", part2(numbers, read));

    return 0;
}
