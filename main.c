#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    // Utilized by newLine and validate functions.
    int size;
    int* array;
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
    int* wordBuffer[15] = {0};

    while (bufSize > 0) { // we will use this to fill our arr until it is max capacity.
        dynamic_array tempArr;
        readChars(&tempArr, inputFD);
        if (bufSize - tempArr.size > 0) {
            bufSize -= tempArr.size;
            arrayRebuild(arr, &tempArr);
        }
        else {
            break;
        }
    }

    fclose(inputFD); 
    printf("Exited loop!\n");
    printf("Char count: %d \n", arr->size);
    printf("Size of array: %d\n", sizeof(arr->array));

    for (int i = 0; i < arr->size; i++) {
        printf("%c", arr->array[i]);
    }

    pthread_exit(1);
}

int main() {

    // Child a (reads from wordlist2.txt, sets aside 300ish chars, prints out)
    // Child b validates user input against the array from child a. (in from terminal, out to terminal, maybe to a third results thread)
    dynamic_array arr = { .size = 0, .array = {' '}};

    pthread_t childA;
    pthread_create(&childA, NULL, newLine, &arr);
    if (pthread_join(childA, NULL)) { 
        // success - now send out child b.
        printf("yooo");
    }
    return 0;
}