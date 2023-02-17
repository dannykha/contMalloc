/*
* Author: Danny Kha
* CSS 430
* Due Date: 2/26/23
* Contiguous Memory Allocation
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

#define MEMSIZE 80
#define MAXCMDS 255
#define EMPTY '.'
#define FIRST 'F'
#define BEST  'B'
#define WORST 'W'

char mem[MEMSIZE];
bool running = true;

// function used to allocate by first-fit algo
void allocFirst(char name[MAXCMDS], int size) {
    int currStart = -1;
    int spaceSize = 0;
    for (int i = 0; i < MEMSIZE; ++i) {
        if (mem[i] == EMPTY) {
            // found start of empty space
            if (currStart == -1) {
                currStart = i;
            }
            spaceSize++;
        }
        else {
            // found empty space that fits the size 
            if (spaceSize >= size) {
                for (int j = 0; j < size; ++j) {
                    mem[currStart + j] = name[0];
                }
                return;
            }
            currStart = -1;
            spaceSize = 0;
        }
    }
    // memory case
    if (spaceSize >= size)
    {
        for (int j = 0; j < size; ++j) {
            mem[currStart + j] = name[0];
        }
    }
}

// function used to allocate by best-fit algo
void allocBest(char name[MAXCMDS], int size) {
    int currStart = -1;
    int bestIndex = -1;
    int currSize = 0;
    int holeSize = 0;
    int bestSize = __INT32_MAX__;
    for (int i = 0; i < MEMSIZE; ++i) {
        if (mem[i] == EMPTY) {
            // found start of empty space
            if (currStart == -1) {
                currStart = i;
            }
        }
        else {
            // found end of empty space prior to allocation
            if (currStart != -1) {
                holeSize = i - currStart;
                if (holeSize >= size && holeSize < bestSize) {
                    bestIndex = currStart;
                    bestSize = holeSize;
                }
                currStart = -1;
            }
        }
    }
    // memory case
    if (currStart != -1) {
        currSize = MEMSIZE - currStart;
        if (currSize >= size && currSize < bestSize) {
            bestIndex = currStart;
            bestSize = currSize;
        }
    }
    if (bestIndex == -1) {
        printf("Error: Out of memory at:  %d\n", size);
        return;
    }
    for (int j = 0; j < size; ++j) {
        mem[bestIndex + j] = name[0];
    }
}

void allocWorst(char name[MAXCMDS], int size) {
    int currStart = 0;
    int currSize = 0;
    int holeCurrSize = 0;
    int largestStart = -1;
    int largestSize = -1;
    bool inHole = false;
    
    for (int i = 0; i < MEMSIZE; ++i) {
        // found the start of empty space
        if (!inHole && mem[i] == EMPTY) {
            inHole = true;
            currStart = i;
        } 
        // found the end of empty space
        else if (inHole && mem[i] != EMPTY) {
            inHole = false;
            holeCurrSize = i - currStart;
            if (holeCurrSize >= size && holeCurrSize > largestSize) {
                // found an empty space large enough
                largestStart = currStart;
                largestSize = holeCurrSize;
            }
        }
    }

    // ensuring that the last portion of memory is valid space
    if (inHole && (MEMSIZE - currStart) >= size) {
        currSize = MEMSIZE - currStart;
        if (currSize > largestSize) {
            largestStart = currStart;
            largestSize = currSize;
        }
    }

    // allocating the memory for the space
    if (largestStart != -1) {
        for (int j = 0; j < size; ++j) {
            mem[j + largestStart] = name[0];
        }
    } 
    else {
        printf("Error: Out of memory at:  %d\n", size);    
    }
}

// function used to choose which allocation algorithm to choose from
void allocate(char name[MAXCMDS], int size, char algo[MAXCMDS]) {
    switch(algo[0]) {
        case FIRST: {
            allocFirst(name, size);
            break;
        }
        case BEST: {
            allocBest(name, size);
            break;
        }
        case WORST: {
            allocWorst(name, size);
            break;
        }
        default: {
            printf("Invalid algorithm \n");
            break;
        }
    }
}

// function used to free space of the name given
void freeSpace(char name[MAXCMDS]) {
    for (int i = 0; i < MEMSIZE; i++) {
        if (mem[i] == name[0]) {
            mem[i] = EMPTY;
        }
    }
}

// function used to print out all the memory currently
void showMem(char* mem) {
    for (int i = 0; i < MEMSIZE; i++) {
        printf("%c", mem[i]);
    }
    printf("\n");
}

// function used to compact all the empty spaces in between in memory
void compact() {
    int temp = 0;
    for (int i = 0; i < MEMSIZE; i++) {
        if (mem[i] != EMPTY) {
            if (temp != i) {
                mem[temp] = mem[i];
                mem[i] = EMPTY;
            }
            temp++;
        }
    }
}

// function to instead of user input, use text file input
void readFile(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    char line[MAXCMDS];
    if (file == NULL) {
        printf("Error: Could not open file %s\n", fileName);
        return;
    }
    while (fgets(line, MAXCMDS, file)) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = 0;
        }
        printf("cmd> %s\n", line);
        if (line[0] == 'A') {
            char* string = strtok(line, " ");
            char name[MAXCMDS];
            int size = 0;
            char algo[MAXCMDS];
            int index = 0;
            while (string != NULL) {
                if (index == 1) {
                    strcpy(name, string);
                } else if (index == 2) {
                    size = atoi(string);
                } else if (index == 3) {
                    strcpy(algo, string);
                }
                index++;
                string = strtok(NULL, " ");
            }
            allocate(name, size, algo);
        } 
        else if (line[0] == 'F') {
            char* string = strtok(line, " ");
            char name[MAXCMDS];
            string = strtok(NULL, " ");
            strcpy(name, string);
            freeSpace(name);
        } 
        else if (line[0] == 'S') {
            showMem(mem);
        } 
        else if (line[0] == 'C') {
            compact();
        } 
        else if (line[0] == 'E') {
            running = false;
            break;
        } 
        else {
            printf("Invalid Line in File \n");
        }
    }
    fclose(file);
}

// main function that continues to run until 'E' is inputted in either console or file
int main() {
    // initalize all the memory to empty space
    for (int a = 0; a < MEMSIZE; a++) {
        mem[a] = '.';
    }
    // main loop
    while (running) {
        fflush(stdout);
        printf("cmd> ");

        char input[MAXCMDS + 1];
        if (fgets(input, MAXCMDS + 1, stdin) == NULL) {
            printf("Invalid input \n");
        } 

        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = 0;
        }

        char* string = strtok(input, " ");
        if (strcmp(string, "A") == 0) {
            char name[MAXCMDS];
            char algo[MAXCMDS];
            int size = 0;
            int index = 0;
            while (string != NULL) {
                if (index == 1) {
                    strcpy(name, string);
                } else if (index == 2) {
                    size = atoi(string);
                } else if (index == 3) {
                    strcpy(algo, string);
                }
                index++;
                string = strtok(NULL, " ");
            }
            allocate(name, size, algo);
        }
        else if (strcmp(string, "F") == 0) {
            char name[MAXCMDS];
            int index = 0;
            while (string != NULL) {
                if (index == 1) {
                    strcpy(name, string);
                }
                index++;
                string = strtok(NULL, " ");
            }
            freeSpace(name);
        }  
        else if (strcmp(string, "S") == 0) {
            showMem(mem);
        } 
        else if (strcmp(string, "R") == 0) {
            char name[MAXCMDS];
            string = strtok(NULL, " ");
            strcpy(name, string);
            readFile(name);
        } 
        else if (strcmp(string, "C") == 0) {
            compact();
        } 
        else if (strcmp(string, "E") == 0) {
            running = false;
            break;
        }
        else {
            printf("Invalid input \n");
        }
    }
}
