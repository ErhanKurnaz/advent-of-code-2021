#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// length of pattern value
#define PATTERN_CAP 11

// length of pattern list
#define PATTERNS_CAP 10
#define OUTPUTS_CAP 4

#define BUFF_CAP PATTERN_CAP * 15
#define SIGNAL_CAP 256

typedef char PatternArray[PATTERNS_CAP][PATTERN_CAP];
typedef char OutputArray[OUTPUTS_CAP][PATTERN_CAP];

struct {
    PatternArray patterns;
    OutputArray outputs;
} typedef Signal;

struct {
    char top;
    char top_left;
    char top_right;
    char middel;
    char bottom_left;
    char bottom_right;
    char bottom;
} typedef Mapping;

void print_signals(Signal* signals, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("Patterns:\t");
        for (int j = 0; j < PATTERNS_CAP; ++j) {
            printf("%s ", signals[i].patterns[j]);
        }
        printf("\n");

        printf("Outputs:\t");
        for (int j = 0; j < OUTPUTS_CAP; ++j) {
            printf("%s ", signals[i].outputs[j]);
        }
        printf("\n\n");
    }
}

void print_mapping(Mapping mapping) {
    printf(" %c%c%c%c \n", mapping.top, mapping.top, mapping.top, mapping.top);
    printf("%c    %c\n", mapping.top_left, mapping.top_right);
    printf("%c    %c\n", mapping.top_left, mapping.top_right);
    printf(" %c%c%c%c \n", mapping.middel, mapping.middel, mapping.middel, mapping.middel);
    printf("%c    %c\n", mapping.bottom_left, mapping.bottom_right);
    printf("%c    %c\n", mapping.bottom_left, mapping.bottom_right);
    printf(" %c%c%c%c \n", mapping.bottom, mapping.bottom, mapping.bottom, mapping.bottom);
}

// Returns a number based on their unique char count
// Returns -1 if no number could be found
int check_length(char* pattern) {
    switch (strlen(pattern)) {
        case 2: return 1;
        case 4: return 4;
        case 3: return 7;
        case 7: return 8;
        default: return -1;
    }
}


int part1(Signal* signals, size_t size) {
    int result = 0;

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < OUTPUTS_CAP; ++j) {
            if (check_length(signals[i].outputs[j]) >= 0) {
                result++;
            }
        }
    }

    return result;
}

int get_difference(char* pattern1, char* pattern2, char* dst) {
    int size = 0;
    char buffer[PATTERN_CAP] = {0};

    char* character1 = pattern1;

    while (*character1) {
        char* character2 = pattern2;

        bool is_included = false;
        while (*character2) {
            if (*character1 == *character2) {
                is_included = true;
                break;
            }

            character2++;
        }

        if (!is_included) {
            buffer[size++] = *character1;
        }

        character1++;
    }
    strcpy(dst, buffer);

    return size;
}

int get_intersection(char* pattern1, char* pattern2, char* dst) {
    int size = 0;
    char buffer[PATTERN_CAP] = {0};

    char* character1 = pattern1;

    while (*character1) {
        char* character2 = pattern2;

        bool is_included = false;
        while (*character2) {

            if (*character1 == *character2) {
                is_included = true;
                break;
            }

            character2++;
        }

        if (is_included) {
            buffer[size++] = *character1;
        }

        character1++;
    }

    strcpy(dst, buffer);

    return size;
}

int get_union(char* pattern1, char* pattern2, char* dst) {
    int size = 0;
    char buffer[PATTERN_CAP] = {0};

    char* character1 = pattern1;
    while (*character1) {
        buffer[size++] = *character1;
        character1++;
    }

    char* character2 = pattern2;
    while (*character2) {
        character1 = pattern1;
        bool is_included = false;
        while (*character1) {
            if (*character1 == *character2) {
                is_included = true;
                break;
            }

            character1++;
        }

        if (!is_included) {
            buffer[size++] = *character2;
        }

        character2++;
    }

    strcpy(dst, buffer);

    return size;
}

Mapping get_mapping(Signal signal) {
    Mapping mapping = {0};
    char one[PATTERN_CAP] = {0};
    char four[PATTERN_CAP] = {0};
    char seven[PATTERN_CAP] = {0};
    char eight[PATTERN_CAP] = {0};

    // first iteration to get all sized digits out for a point of reference
    for (size_t i = 0; i < PATTERNS_CAP; ++i) {
        switch(check_length(signal.patterns[i])) {
            case -1:
                break;
            case 1:
                strcpy(one, signal.patterns[i]);
                break;
            case 4:
                strcpy(four, signal.patterns[i]);
                break;
            case 7:
                strcpy(seven, signal.patterns[i]);
                break;
            case 8:
                strcpy(eight, signal.patterns[i]);
                break;
            default:
                fprintf(stderr, "Unreachable\n");
                exit(1);
        }
    }

    char seven_one_diff[PATTERN_CAP] = {0};
    get_difference(seven, one, seven_one_diff);
    mapping.top = seven_one_diff[0];

    // second iteration getting the other numbers
    for (size_t i = 0; i < PATTERNS_CAP; ++i) {
        char* pattern = signal.patterns[i];
        if (check_length(pattern) != -1) {
            continue;
        }

        char diff_four[PATTERN_CAP] = {0};
        int size = get_difference(four, pattern, diff_four);
        if (size == 1) {
            if (strlen(pattern) == 6) {
                char diff_one[PATTERN_CAP] = {0};
                int diff_one_size = get_difference(one, pattern, diff_one);

                if (diff_one_size == 1) {
                    // we found 6
                    mapping.top_right = *diff_one;
                    char top_right[] = {*diff_one, '\0'};
                    char diff_tr_one[PATTERN_CAP] = {0};
                    get_difference(one, top_right, diff_tr_one);
                    mapping.bottom_right = *diff_tr_one;
                    continue;
                }

                assert(diff_one_size == 0 && "Diff one size wasn't zero\n");

                char seven_four_union[PATTERN_CAP] = {0};
                get_union(seven, four, seven_four_union);

                char union_diff[PATTERN_CAP] = {0};
                int union_diff_size = get_difference(seven_four_union, pattern, union_diff);
                if (union_diff_size == 1) {
                    // we found 0
                    mapping.middel = *diff_four;
                    continue;
                }

                continue;
            }

            if (strlen(pattern) == 5) {
                char union_diff[PATTERN_CAP] = {0};
                int one_size = get_difference(one, pattern, union_diff);

                if (one_size == 0) {
                    // we found 3
                    mapping.top_left = diff_four[0];
                    continue;
                }

                continue;
            }
        }

        char seven_four_union[PATTERN_CAP] = {0};
        get_union(seven, four, seven_four_union);

        char union_diff[PATTERN_CAP] = {0};
        int union_diff_size = get_difference(pattern, seven_four_union, union_diff);

        if (union_diff_size == 1) {
            // we found 9
            mapping.bottom = *union_diff;
            char diff_eight[PATTERN_CAP] = {0};
            get_difference(eight, pattern, diff_eight);
            mapping.bottom_left = *diff_eight;
        }
    }

    return mapping;
}

int get_from_mapping(char* pattern, Mapping mapping) {
    // lookup table for all characters
    char lookup[][PATTERN_CAP] = {
        { mapping.top, mapping.top_left, mapping.top_right, mapping.bottom_left, mapping.bottom_right, mapping.bottom, '\0' },
        { mapping.top_right, mapping.bottom_right },
        { mapping.top, mapping.top_right, mapping.middel, mapping.bottom_left, mapping.bottom, '\0' },
        { mapping.top, mapping.top_right, mapping.middel, mapping.bottom_right, mapping.bottom, '\0' },
        { mapping.top_left, mapping.top_right, mapping.middel, mapping.bottom_right, '\0' },
        { mapping.top, mapping.top_left, mapping.middel, mapping.bottom_right, mapping.bottom, '\0' },
        { mapping.top, mapping.top_left, mapping.middel, mapping.bottom_left, mapping.bottom_right, mapping.bottom, '\0' },
        { mapping.top, mapping.top_left, mapping.bottom_right, '\0' },
        { mapping.top, mapping.top_left, mapping.top_right, mapping.middel, mapping.bottom_left, mapping.bottom_right, mapping.bottom, '\0' },
        { mapping.top, mapping.top_left, mapping.top_right, mapping.middel, mapping.bottom_right, mapping.bottom, '\0' },
    };

    for (int i = 0; i < 10; ++i) {
        char diff[PATTERN_CAP] = {0};
        int size = get_difference(pattern, lookup[i], diff);

        if (size != 0) {
            continue;
        }

        diff[0] = '\0';
        size = get_difference(lookup[i], pattern, diff);

        if (size != 0) {
            continue;
        }

        return i;
    }

    return -1;
}

int part2(Signal* signals, size_t size) {
    int result = 0;

    for (size_t i = 0; i < size; ++i) {
        Mapping mapping = get_mapping(signals[i]);

        int number = 0;

        for (size_t j = 0; j < OUTPUTS_CAP; ++j) {
            int digit;
            int sized_digit = check_length(signals[i].outputs[j]);

            if (sized_digit >= 0) {
                digit = sized_digit;
            } else {
                digit = get_from_mapping(signals[i].outputs[j], mapping);
                assert(digit >= 0);
            }

            number += digit * (int) pow(10, OUTPUTS_CAP - (j + 1));
        }

        result += number;
    }

    return result;
}

int main(void) {
    const char* file_name = "./input.txt";

    FILE* f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'\n", file_name);
        exit(1);
    }

    char buffer[BUFF_CAP];
    Signal signals[SIGNAL_CAP] = {0};
    size_t size = 0;

    while(fgets(buffer, BUFF_CAP, f)) {
        char* start = buffer;

        Signal signal = {0};
        int i = 0;
        bool parsing_outputs = false;
        char pattern_buffer[PATTERN_CAP] = {0};
        int buff_index = 0;
        while (*start) {
            if (*start == ' ' || *start == '\n') {
                if (buff_index != 0) {
                    if (parsing_outputs) {
                        memcpy(signal.outputs[i], pattern_buffer, sizeof(pattern_buffer));
                    } else {
                        memcpy(signal.patterns[i], pattern_buffer, sizeof(pattern_buffer));
                    }

                    memset(pattern_buffer, 0, sizeof(pattern_buffer));
                    i++;
                    buff_index = 0;
                }

                start++;
                continue;
            }

            if (*start == '|') {
                parsing_outputs = true;
                start++;
                i = 0;
                continue;
            }

            pattern_buffer[buff_index++] = *start;

            start++;
        }

        signals[size++] = signal;
    }

    fclose(f);

    //print_signals(signals, size);
    printf("part1:\t%d\n", part1(signals, size));
    printf("part2:\t%d\n", part2(signals, size));
    return 0;
}
