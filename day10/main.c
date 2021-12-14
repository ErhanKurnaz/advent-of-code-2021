#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define TEST 0

#if TEST
#define FILE_NAME "input.test.txt"
#else
#define FILE_NAME "input.txt"
#endif

#define CHUNK_CAP 256
#define CHUNKS_CAP 128

#define IS_OPENING(character) (                             \
        character == '['                                    \
        || character == '('                                 \
        || character == '{'                                 \
        || character == '<'                                 \
    )

#define IS_CLOSING(character) (                             \
        character == ']'                                    \
        || character == ')'                                 \
        || character == '}'                                 \
        || character == '>'                                 \
    )

enum {
    None = 0,
    Incomplete,
    Syntax,
} typedef Error;


struct Chunk {
    char opening;
    char closing;
    Error error;
    struct Chunk** children;
    int children_size;
};

typedef struct Chunk Chunk;

// credit: https://www.edureka.co/blog/sorting-algorithms-in-c/
void insertion_sort(uint64_t array[], int n) {
    for (int i = 1; i < n; i++) {
        uint64_t element = array[i];
        int j = i - 1;
        while (j >= 0 && array[j] > element) {
            array[j + 1] = array[j];
            j--;
        }

        array[j + 1] = element;
    }
}

int syntax_score_lookup['}' + 1] = {
    [')'] = 3,
    [']'] = 57,
    ['}'] = 1197,
    ['>'] = 25137,
};

int auto_score_lookup['}' + 1] = {
    [')'] = 1,
    [']'] = 2,
    ['}'] = 3,
    ['>'] = 4,
};

char get_right_closing(char opening) {
    switch (opening) {
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        case '<':
            return '>';
        default:
            return '\0';
    }
}

bool is_right_closing(char opening, char closing) {
    return get_right_closing(opening) == closing;
}

Chunk* parse_chunk(char* startp, char** endp) {
    char* character = startp;
    Chunk* chunk = malloc(sizeof(Chunk));
    Chunk** chunk_children = malloc(sizeof(Chunk*) * (CHUNK_CAP / 2));
    int child_size = 0;
    bool parse_opening = true;

    while (*character) {
        if (IS_OPENING(*character)) {
            if (parse_opening) {
                chunk->opening = *character;
                parse_opening = false;
            } else {
                char* end;
                chunk_children[child_size++] = parse_chunk(character, &end);
                character = end + 1;
                continue;
            }
        } else if (IS_CLOSING(*character)) {
            chunk->closing = *character;
            break;
        }

        character++;
    }

    if (endp != NULL) {
        *endp = character;
    }

    chunk->children = chunk_children;
    chunk->children_size = child_size;

    if (!chunk->closing) {
        chunk->error = Incomplete;
        chunk->closing = get_right_closing(chunk->opening);
    } else if (!is_right_closing(chunk->opening, chunk->closing)) {
        chunk->error = Syntax;
    }

    return chunk;
}

void free_chunk(Chunk* chunk) {
    if (chunk->children_size > 0) {
        for (int i = 0; i < chunk->children_size; ++i) {
            free_chunk(chunk->children[i]);
        }
    }
    free(chunk);
}

void free_chunks(Chunk** chunks, int size) {
    for (int i = 0; i < size; ++i) {
        free_chunk(chunks[i]);
    }
}

void print_chunk_n(Chunk* chunk, bool newline) {
    printf("%c", chunk->opening);
    if (chunk->children_size > 0) {
        for (int i = 0; i < chunk->children_size; ++i) {
            printf(" ");
            print_chunk_n(chunk->children[i], false);
            if (i != chunk->children_size - 1) {
                printf(",");
            }

            printf(" ");
        }
    }
    if (chunk->closing) {
        printf("%c", chunk->closing);
    } else {
        printf("|");
    }

    if (newline) {
        printf("\n");
    }
}

void print_chunk(Chunk* chunk) {
    print_chunk_n(chunk, true);
}

void print_chunks(Chunk** chunks, int size) {
    for (int i = 0; i < size; ++i) {
        print_chunk(chunks[i]);
    }
    printf("\n");
}

uint64_t get_error_score(Chunk* chunk) {
    uint64_t result = 0;
    if (chunk->error == Syntax) {
        result += syntax_score_lookup[(int) chunk->closing];
    }

    for (int i = 0; i < chunk->children_size; ++i) {
        result += get_error_score(chunk->children[i]);
    }

    return result;
}

uint64_t get_auto_score(Chunk* chunk) {
    uint64_t result = 0;

    if (chunk->error == Syntax) {
        return 0;
    }

    for (int i = 0; i < chunk->children_size; ++i) {
        result += get_auto_score(chunk->children[i]);
    }

    if (chunk->error == Incomplete) {
        result *= 5;
        result += auto_score_lookup[(int) chunk->closing];
    }

    return result;
}

uint64_t part1(char lines[CHUNKS_CAP][CHUNK_CAP], int size) {
    uint64_t result = 0;
    for (int i = 0; i < size; ++i) {
        char* start = lines[i];

        while(*start) {
            char* end;
            Chunk* chunk = parse_chunk(start, &end);

            result += get_error_score(chunk);

            free_chunk(chunk);
            if (!*end) {
                break;
            }

            start = end + 1;
        }
    }

    return result;
}

uint64_t part2(char lines[CHUNKS_CAP][CHUNK_CAP], int size) {
    uint64_t results[CHUNK_CAP / 2] = {0};
    int result_size = 0;
    for (int i = 0; i < size; ++i) {
        char* start = lines[i];
        uint64_t line_result = 0;

        while(*start) {
            char* end;
            Chunk* chunk = parse_chunk(start, &end);
            if (get_error_score(chunk) == 0) {
                line_result += get_auto_score(chunk);
            }

            free_chunk(chunk);
            if (!*end) {
                break;
            }

            start = end + 1;
        }

        if (line_result > 0) {
            results[result_size++] = line_result;
        }
    }

    insertion_sort(results, result_size);

    return results[result_size / 2];
}

int main(void) {
    FILE* f = fopen("./"FILE_NAME, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'\n", FILE_NAME);
        exit(1);
    }

    char buffer[CHUNK_CAP] = {0};
    char chunks[CHUNKS_CAP][CHUNK_CAP] = {0};
    int size = 0;

    while (fgets(buffer, sizeof(buffer), f)) {
        strncpy(chunks[size++], buffer, strlen(buffer) - 1);
    }

    fclose(f);

    printf("part1:\t%lld\n", part1(chunks, size));
    printf("part2:\t%lld\n", part2(chunks, size));

    return 0;
}
