#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    // Utilized by newLine and validate functions.
    int size;
    char* array;
} dynamic_array;

typedef struct {
    // shared struct to tell readWords to send to newLine. May be extended for Child c action.
    int ready_signal;
} signals;

int newLine(dynamic_array*, signals*);
int readChars(dynamic_array*);
void shortenArray(dynamic_array*, int, char*);
//int findTermination(char*);
void arrayRebuild(dynamic_array*, char*, int);
void arrayCompletion(dynamic_array*);

int main() {
    dynamic_array* init;
    signals* firstOne;

    int result = newLine(init, firstOne);
    return 0;
}



int newLine(dynamic_array* arr, signals* sig) {
    // Child a (reads from wordlist2.txt, sets aside 300ish chars, prints out)
    // Child b validates user input against the array from child a. (in from terminal, out to terminal, maybe to a third results thread)

    // would be best to just use the getc() function for reading chars, and check if its \n. Page 532 on textbook.

    FILE* inputFD = fopen("wordlist2.txt", "r");
    int bufSize = 300; // bufferSize
    char* wordBuffer[15] = {0};
    int returnedByteCount;
    int index;
    while (bufSize > 0) { // we will use this to fill our arr until it is max capacity.
    // OLD IMPLEMENTATION:
    //     printf("%d \n", bufSize);
    //     returnedByteCount = fread(&wordBuffer, 15, 1, inputFD); 
    //     // ^^ We need to remove findTermination and make this a do while loop with getchar()
    //     if (returnedByteCount <= 0) { // > 0 response is successful operation.
    //         printf("%d", returnedByteCount);
    //         printf("Read Failure.");
    //         return -1;
    //     }
    //     else if (bufSize - returnedByteCount >= 0) {
    //         index = findTermination(wordBuffer);
    //         char* wordBufferReduced = (char*) malloc (index);
    //         for (int i = 0; i < index; i++) { // creates a stripped (removes any extra spaces, \n, and null terminator) array 
    //             wordBufferReduced[i] = wordBuffer[i];
    //         }
    //         arrayRebuild(arr, wordBufferReduced, index); // appends this new word to the array rebuild.
    //         bufSize -= index;
    //     }
    //     else {
    //         bufSize = 0;
    //         break;
    //     }
    //     memset(wordBuffer, 0, 15); // resets wordBuffer values to all 0
    // }
        returnedByteCount = readChars(arr);
        // handle the reduction in bufSize and also write error handling / error codes. 
    }
    fclose(inputFD); 
    arrayCompletion(arr);
    printf("Exited loop!\n");
    printf("Char count: %d \n", arr->size);
    // now we can start to display all of the information.
    //for (int i = 0; i < arr->size; i++) {
        //printf("%c \n", arr->array[i]);
    //}
    sig->ready_signal = 1; // indicates this process has completed its job.
    return 1;
}

int readChars(dynamic_array* arr) { // will return the number of bytes read.
    dynamic_array* newArr;
    int index = 0;
    int currentChar;
    char* tempBuffer[15];
    while (index < 15) { // words have hard limit of 15 chars
        currentChar = getchar();
        if (currentChar != '\n' || currentChar != '\0') { // only terminators for a line/word
            tempBuffer[index] = currentChar;
        }
        else {
            break;
        }
        index++;
    } 
    shortenArray(newArr, index, tempBuffer); // makes new array with proper size
    arrayRebuild(arr, newArr, index); // appends to the larger array

    return index; // return number of chars (bytes) read.
}

void shortenArray(dynamic_array* arr_pointer, int index, char* tempBuffer) {
    arr_pointer->size = index;
    arr_pointer->array = (char*) calloc(arr_pointer->size, 1);
    for (int i = 0; i < arr_pointer->size; i++) {
        arr_pointer->array[i] = tempBuffer[i];
    }
}
// no longer used. fault in idea.
// int findTermination(char* wordBuffer) { // Can't see why this would fail.
//     for (int i = 0; i < 15; i++) {
//         if (wordBuffer[i] == '\0' || wordBuffer[i] == '\n' || wordBuffer[i] == '\t') { // only terminator cases I can think of 
//             printf("Found problem: %c \n", wordBuffer[i]);
//             return i; // end 1 before the terminator char if we find them.
//         }
//         else {
//             printf("%c", wordBuffer[i]);
//         }
//     }
//     printf("\n");
//     return 14; // otherwise it made it to the max length.
// }

void arrayRebuild(dynamic_array* arr_pointer, char* arr2, int index) {
    // arg1 is existing array, arg2 is new array, arg3 is length of new array.
    dynamic_array *newArray = arr_pointer;
    int oldSize = arr_pointer->size;
    int newSize = oldSize + index;
    arr_pointer->array = (char*) calloc(newSize + 1, 1); // callocs to new size.

    // These 2 loops write all of the existing and new entries to the array.
    for (int i = 0; i < oldSize; i++) { // adds all of the old words
        arr_pointer->array[i] = (char) newArray->array[i];
    }
    arr_pointer->array[arr_pointer->size] = ' '; // adds a space before the new word.
    int shift = oldSize + 1;
    for (int i = 0; i < index; i++) {
        arr_pointer->array[i+shift] = (char) arr2[i]; // shift total array forward by old count, since we are appending new array.
    }
    arr_pointer->size = newSize + 1; // size + 1 to handle the space we added.
}

void arrayCompletion(dynamic_array* arr) { // Should work.
    dynamic_array* newArray = arr;
    arr->array = (char*) calloc(newArray->size + 1, 1); // increment by one to handle the null terminator

    for (int i = 0; i < arr->size; i++) {
        arr->array[i] = newArray->array[i]; // remaps the values to new array
    }
    arr->array[arr->size + 1] = '\0';
    arr->size ++;
}