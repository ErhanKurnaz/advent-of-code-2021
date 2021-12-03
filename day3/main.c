#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define BUFF_CAP 256
#define NUM_CAP 1024
#define TEST 0

#if TEST
#define BYTES_TO_READ 5
#else
#define BYTES_TO_READ 12
#endif

void get_bit_totals(unsigned int* numbers, int size, int *result) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < BYTES_TO_READ; ++j) {
            result[j] += (numbers[i] >> (BYTES_TO_READ - (j + 1))) & 1;
        }
    }
}

int part1(unsigned int* numbers, int size) {
    int totals[BYTES_TO_READ] = {0};
    get_bit_totals(numbers, size, totals);

    int gamma_rate = 0;
    int epsilon_rate = 0;
    for (int i = 0; i < BYTES_TO_READ; ++i) {
        int insert_val = 0;

        if (totals[i] > size / 2) {
            insert_val = 1;
        }

        gamma_rate = gamma_rate | (insert_val << (BYTES_TO_READ - (i + 1)));
        epsilon_rate = epsilon_rate | (!insert_val << (BYTES_TO_READ - (i + 1)));
    }

    return gamma_rate * epsilon_rate;
}

int get_oxygen_rating(unsigned int* numbers, int size, int position) {
    if (size == 1) {
        return numbers[0];
    }

    assert(position < BYTES_TO_READ && "position is greater than bytes to read");
    int totals[BYTES_TO_READ] = {0};
    get_bit_totals(numbers, size, totals);
    unsigned int new_numbers[NUM_CAP] = {0};
    int new_size = 0;

    int insert_val = 0;

    // Add 1 if there were more 1 than 0
    // we check that by comparing the amount of 1 to the length of the array divided by 2
    // however if the size is an even number and the amount of 1 and 0 are equal
    // then we have to choose 1
    if (totals[position] > size / 2 || (size % 2 == 0 && totals[position] == size / 2)) {
        insert_val = 1;
    }

    for (int i = 0; i < size; ++i) {
        int bit = numbers[i] >> (BYTES_TO_READ - (position + 1)) & 1;
        if (bit == insert_val) {
            new_numbers[new_size++] = numbers[i];
        }
    }

    return get_oxygen_rating(new_numbers, new_size, position + 1);
}

int get_scrubber_rating(unsigned int* numbers, int size, int position) {
    if (size == 1) {
        return numbers[0];
    }

    assert(position < BYTES_TO_READ && "position is greater than bytes to read");
    int totals[BYTES_TO_READ] = {0};
    get_bit_totals(numbers, size, totals);
    unsigned int new_numbers[NUM_CAP] = {0};
    int new_size = 0;

    int insert_val = 0;
    // Add 1 if there were more 1 than 0
    // we check that by comparing the amount of 1 to the length of the array divided by 2
    // also if the numbers are the same but the size is not an even number
    // then that means that there was less 1 than 0
    if (totals[position] < size / 2 || (size % 2 != 0 && totals[position] == size / 2)) {
        insert_val = 1;
    }

    for (int i = 0; i < size; ++i) {
        int bit = numbers[i] >> (BYTES_TO_READ - (position + 1)) & 1;
        if (bit == insert_val) {
            new_numbers[new_size++] = numbers[i];
        }
    }

    return get_scrubber_rating(new_numbers, new_size, position + 1);
}

int part2(unsigned int* numbers, int size) {
    return get_oxygen_rating(numbers, size, 0) * get_scrubber_rating(numbers, size, 0);
}

int main(void) {
    #if TEST
    const char *file_name = "./input.test.txt";
    #else
    const char *file_name = "./input.txt";
    #endif
    FILE *f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'", file_name);
        exit(1);
    }

    char buffer[BUFF_CAP];
    unsigned int numbers[NUM_CAP];
    int read = 0;
    while (fgets(buffer, BUFF_CAP, f)) {
        numbers[read++] = strtol(buffer, (char **)NULL, 2);
    }

    fclose(f);

    printf("part1:\t%d\n", part1(numbers, read));
    printf("part2:\t%d\n", part2(numbers, read));

    return 0;
}
