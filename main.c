#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct {
    int size;
    int* array;
    int wordCount;
} dynamic_array;

void readChars(dynamic_array* arr_pointer, FILE* fdin) {
    // will return the number of actual useful bytes read.
    int index = 0;
    int currentChar;
    while (index < 15) { // words have hard limit of 15 chars
        currentChar = getc(fdin);
        if (currentChar == '\n' || currentChar == '\0') { // only terminators for a line/word
            break;
        }
        else {
            arr_pointer->array[index] = currentChar;
            index++;
        }
    }
    arr_pointer->array[index] = ' ';
    arr_pointer->size = index;
    index = 0;
}

void arrayRebuild(dynamic_array* arr_og, dynamic_array* arr_new) {
    int newSize = arr_og->size + arr_new->size + 1;
    int* tempArray = calloc(newSize, sizeof(int));
    for (int i = 0; i < arr_og->size; i++) {
        tempArray[i] = arr_og->array[i];
    }
    for (int j = 0; j < arr_new->size+1; j++) {
        tempArray[j+arr_og->size] = arr_new->array[j];
    }
    arr_og->array = tempArray;
    arr_og->size = newSize;
}

int newLine(dynamic_array* arr) {
    FILE* inputFD = fopen("wordlist2.txt", "r");
    int bufSize = 50;

    while (bufSize > 0) { // we will use this to fill our arr until it is max capacity.
        dynamic_array tempArr = {.size = 0, .array = calloc(1, sizeof(int))};
        readChars(&tempArr, inputFD);
        if (bufSize - tempArr.size > 0) {
            bufSize -= tempArr.size;
            arr->wordCount += 1;
            arrayRebuild(arr, &tempArr);
        }
        else {
            break;
        }
    }
    fclose(inputFD);
    arr->size--; // this will account for the final character being a space due to readChars always appending.
    for (int i = 0; i < arr->size; i++) {
        printf("%c", arr->array[i]);
    }
    printf("\n");
    return 0;
}

int checkChar(dynamic_array* arr, int index, int value) {
    int result = (int) arr->array[index] == value;
    if (result) {
        printf(ANSI_COLOR_GREEN "%c" ANSI_COLOR_RESET, value);
    }
    else {
        printf(ANSI_COLOR_RED "%c" ANSI_COLOR_RESET, value);
    }
    return result;
}

int validateRespones(dynamic_array* arr) {
    int* resultArray = calloc(arr->size, sizeof(int));
    int currentIndex = 0;
    while (currentIndex < arr->size) {
        int character = getch();
        if (character == 64) { // exit
            return -1;
        }
        else if (character == 8) { // backspace
            if (currentIndex > 0) { // if index is 0, ignores
                printf("\b");
                currentIndex--;
            }
        }
        else { // do 
            resultArray[currentIndex] = checkChar(arr, currentIndex, character);
            currentIndex++;
        }
    }
    printf("\n");
    printf("Finished!");
    for (int i = 0; i < arr->size; i++) { 
        printf("%d", resultArray[i]);
    }
    return 0;
}

int main() {
    // Child a (reads from wordlist2.txt, sets aside 300ish chars, prints out)
    // Child b validates user input against the array from child a. (in from terminal, out to terminal, maybe to a third results thread)
    dynamic_array arr = { .size = 0, .array = calloc(1, sizeof(int))};
    newLine(&arr);
    validateRespones(&arr);
    return 0;
}