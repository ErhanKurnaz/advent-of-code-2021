#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define TEST 0

#if TEST
#define INPUT_FILE "input.test.txt"
#else
#define INPUT_FILE "input.txt"
#endif

#define CAVE_NAME_CAP 6
#define BUFF_CAP CAVE_NAME_CAP * 2 + 2
#define CAVES_CAP 256

bool generate_graph = false;

struct {
    char name[CAVE_NAME_CAP];
    int position;
    int links[CAVES_CAP];
    int link_size;
    bool big;
} typedef Cave;

void cmd(char** args) {
    pid_t child = fork();

    if (child == 0) {
        // we are now in child
        if (execvp(args[0], args) < 0) {
            fprintf(stderr, "Could not execute child: %s\n", strerror(errno));
            exit(1);
        }
    } else if (child > 0) {
        int status;
        if (wait(&status) < 0) {
            fprintf(stderr, "Could not wait for forked child: %s\n", strerror(errno));
            exit(1);
        }
    } else {
        fprintf(stderr, "Could not fork child: %s\n", strerror(errno));
        exit(1);
    }
}

int find_cave_by_name(const Cave* caves, int size, const char* name) {
    for (int i = 0; i < size; ++i) {
        if (strcmp(caves[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

void dump_dot(FILE* f, Cave cave, Cave* caves, bool* visited) {
    visited[cave.position] = true;
    char* shape = NULL;

    if (strcmp(cave.name, "start") == 0 || strcmp(cave.name, "end") == 0) {
        shape = "tripleoctagon";
    } else if (cave.big) {
        shape = "diamond";
    } else {
        shape = "oval";
    }

    fprintf(f, "    %s [shape=%s]\n", cave.name, shape);
    for (int i = 0; i < cave.link_size; ++i) {
        if (!visited[cave.links[i]]) {
            fprintf(f, "    %s -- %s\n", cave.name, caves[cave.links[i]].name);
        }
    }

    for (int i = 0; i < cave.link_size; ++i) {
        if (!visited[cave.links[i]]) {
            dump_dot(f, caves[cave.links[i]], caves, visited);
        }
    }
}

void generate_caves_graph(Cave* caves, int size) {
    char* file_name = "caves.dot";
    printf("Generating graph %s\n", file_name);
    FILE* f = fopen(file_name, "w");

    if (f == NULL) {
        fprintf(stderr, "Could not create file '%s'\n", file_name);
        exit(1);
    }

    bool visited[CAVES_CAP] = {0};

    fprintf(f, "graph Caves {\n");
    dump_dot(f, caves[find_cave_by_name(caves, size, "start")], caves, visited);

    fprintf(f, "}\n");

    fclose(f);

    char* command[] = {
        "dot",
        "-Tsvg",
        file_name,
        "-O",
        NULL,
    };

    cmd(command);

    printf("Generated graph %s\n", file_name);

}

uint64_t explore_cave(const Cave cave, const Cave* caves, bool* visited) {
    if (strcmp(cave.name, "end") == 0) {
        return 1;
    }

    uint64_t result = 0;
    bool visited_state[CAVES_CAP] = {0};
    memcpy(visited_state, visited, sizeof(visited_state));
    visited_state[cave.position] = true;

    for (int i = 0; i < cave.link_size; ++i) {
        if (!visited_state[cave.links[i]] || caves[cave.links[i]].big) {
            result += explore_cave(caves[cave.links[i]], caves, visited_state);
        }
    }

    return result;
}

uint64_t explore_cave2(const Cave cave, const Cave* caves, bool* visited, bool twice) {
    if (strcmp(cave.name, "end") == 0) {
        return 1;
    }

    uint64_t result = 0;
    bool visited_state[CAVES_CAP] = {0};
    memcpy(visited_state, visited, sizeof(visited_state));
    visited_state[cave.position] = true;

    for (int i = 0; i < cave.link_size; ++i) {
        if (strcmp(caves[cave.links[i]].name, "start") == 0) {
            continue;
        }

        if ((!visited_state[cave.links[i]] || caves[cave.links[i]].big)) {
            result += explore_cave2(caves[cave.links[i]], caves, visited_state, twice);
        } else if (!twice) {
            result += explore_cave2(caves[cave.links[i]], caves, visited_state, true);
        }
    }

    return result;
}

uint64_t part1(const Cave* caves, int size) {
    int start_pos = find_cave_by_name(caves, size, "start");
    Cave start = caves[start_pos];
    bool visited[CAVES_CAP] = {0};

    return explore_cave(start, caves, visited);
}

uint64_t part2(const Cave* caves, int size) {
    int start_pos = find_cave_by_name(caves, size, "start");
    Cave start = caves[start_pos];
    bool visited[CAVES_CAP] = {0};

    return explore_cave2(start, caves, visited, false);
}

int main(int argc, char** argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "--graph") == 0) {
            generate_graph = true;
        }
    }

    FILE* f = fopen("./"INPUT_FILE, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not open file '%s'\n", INPUT_FILE);
        exit(1);
    }

    char buffer[BUFF_CAP] = {0};
    Cave caves[CAVES_CAP] = {0};
    int size = 0;

    while(fgets(buffer, sizeof(buffer), f)) {
        bool parsing_rhs = false;
        char* start = buffer;

        char name1[CAVE_NAME_CAP] = {0};
        char name2[CAVE_NAME_CAP] = {0};
        while(*start && *start != '\n') {
            if (*start == '-') {
                parsing_rhs = true;
                start++;
                continue;
            }

            if (parsing_rhs) {
                name2[strlen(name2)] = *start;
            } else {
                name1[strlen(name1)] = *start;
            }

            start++;
        }

        int pos1 = find_cave_by_name(caves, size, name1);
        if (pos1 == -1) {
            Cave cave = {0};
            cave.position = size;
            cave.big = isupper(name1[0]);
            strcpy(cave.name, name1);
            caves[size] = cave;
            pos1 = size;
            size++;
        }

        int pos2 = find_cave_by_name(caves, size, name2);
        if (pos2 == -1) {
            Cave cave = {0};
            cave.position = size;
            cave.big = isupper(name2[0]);
            strcpy(cave.name, name2);
            caves[size] = cave;
            pos2 = size;
            size++;
        }

        caves[pos1].links[caves[pos1].link_size++] = pos2;
        caves[pos2].links[caves[pos2].link_size++] = pos1;
    }

    fclose(f);

    if (generate_graph) {
        generate_caves_graph(caves, size);
    }

    printf("part1:\t%lld\n", part1(caves, size));
    printf("part2:\t%lld\n", part2(caves, size));

    return 0;
}
