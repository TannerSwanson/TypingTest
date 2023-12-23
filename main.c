#include <stdio.h>

typedef struct {
    // Utilized by newLine and validate functions.
    int size;
    char* array;
} dynamic_array;

typedef struct {
    // shared struct to tell readWords to send to newLine. May be extended for Child c action.
    int ready_signal;
} signals;

int newLine(dynamic_array* arr, signals* sig) {
    // This works, however we have an FD problem. 
    // Child a (reads from wordlist2.txt, sets aside 300ish chars, prints out)
    // Child b validates user input against the array from child a. (in from terminal, out to terminal, maybe to a third results thread)

    int fd[2]; // need to actually map the FD's. 
    int bufSize = 300; // bufferSize
    char* wordBuffer[15] = {0};
    int returnedByteCount;
    int index;
    while (bufSize > 0) { // we will use this to fill our arr until it is max capacity.
        returnedByteCount = read(fd[0], &wordBuffer, 15); // read data from wordlist2.txt
        if (returnedByteCount <= 0) { // > 0 response is successful operation.
            printf("Read Failure.");
            return -1;
        }
        else if (bufSize - returnedByteCount >= 0) {
            index = findTermination(&wordBuffer);
            char* wordBufferReduced = (char) malloc (index);
            for (int i = 0; i < index; i++) { // creates a stripped (removes any extra spaces, \n, and null terminator) array 
                wordBufferReduced[i] = wordBuffer[i];
            }
            arrayRebuild(&arr, &wordBufferReduced); // appends this new word to the array rebuild.
        }
        else {
            bufSize = 0;
        }
        memset(wordBuffer, 0, 15); // resets wordBuffer values to all 0
    }
    arrayTerminator(&arr);
    // now we can start to display all of the information.
    for (int i = 0; i < arr->size; i++) {
        printf("%c", arr->array[i]);
        printf("\n");
    }
    sig->ready_signal = 1; // indicate to other process that we are good to go. might be useless?
}

int findTermination(char* wordBuffer) { // Can't see why this would fail.
    for (int i = 0; i < 15; i++) {
        if (wordBuffer[i] == '\0' || wordBuffer[i] == '\n' || wordBuffer[i] == '\t') { // only terminator cases I can think of 
            return (i - 1); // end 1 before the terminator char if we find them.
        }
    }
    return 14; // otherwise it made it to the max length.
}

void arrayRebuild(dynamic_array* arr_pointer, char* arr2) { // I think this should work ?
    dynamic_array *newArray = arr_pointer;
    int newSize = arr_pointer->size + sizeof(arr2);
    arr_pointer->array = (dynamic_array*) calloc(newSize + 1); // callocs to new size.

    // These 2 loops write all of the existing and new entries to the array.
    for (int i = 0; i < newArray->size; i++) { // adds all of the old words
        arr_pointer->array[i] = (char) newArray->array[i];
    }
    arr_pointer->array[arr_pointer->size] = ' '; // adds a space before the new word.
    for (int i = newArray->size+1; i < newArray->size + sizeof(arr2)+1; i++) { // adds the new word.
        arr_pointer->array[i] = (char) arr2[i];
    }
    arr_pointer->size = newSize + 1; // size + 1 to handle the space we added.
}

void arrayTerminator(dynamic_array* arr) { // Should work.
    dynamic_array* newArray = arr;
    arr->array = (dynamic_array*) malloc(newArray->size + 1); // increment by one to handle the null terminator

    for (int i = 0; i < arr->size; i++) {
        arr->array[i] = newArray->array[i]; // remaps the values to new array
    }
    arr->array[arr->size + 1] = '\0';
}