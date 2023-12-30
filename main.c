#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    // Utilized by newLine and validate functions.
    int size;
    int wordCount;
    char* array;
} dynamic_array;

typedef struct {
    // shared struct to tell readWords to send to newLine. May be extended for Child c action.
    int ready_signal;
} signals;

int newLine(dynamic_array*, signals*);
int readChars(char*, FILE*);
void shortenArray(dynamic_array*, int, char*);
//int findTermination(char*);
void arrayRebuild(dynamic_array*, char*, int);
void arrayCompletion(dynamic_array*);

int main() {
    dynamic_array* init = (dynamic_array*) malloc(sizeof(dynamic_array));
    init->size = 0;
    signals* firstOne = (signals*) malloc(sizeof(signals));

    int result = newLine(init, firstOne);
    return 0;
}



int newLine(dynamic_array* arr, signals* sig) {
    // Child a (reads from wordlist2.txt, sets aside 300ish chars, prints out)
    // Child b validates user input against the array from child a. (in from terminal, out to terminal, maybe to a third results thread)

    // would be best to just use the getc() function for reading chars, and check if its \n. Page 532 on textbook.

    FILE* inputFD = fopen("wordlist2.txt", "r");
    int bufSize = 50; // bufferSize
    char* wordBuffer[15] = {0};
    int returnedByteCount;
    int index;
    while (bufSize > 0) { // we will use this to fill our arr until it is max capacity.
        dynamic_array* tempArr;
        char tempBuffer[15];
        returnedByteCount = readChars(tempBuffer, inputFD);
        printf("Buffer Size: %d, Returned byte count: %d \n", bufSize, returnedByteCount);
        if (bufSize - returnedByteCount > 0) {
            bufSize -= returnedByteCount;

            // REAL VS PREDICTED SIZES ARE LIKE LEGIT WRONG IN THESE 2 FUNCTIONS. MAYBE VARIABLE TYPE SIZE RELATED
            shortenArray(tempArr, returnedByteCount, tempBuffer);
            printf("TempArr size: %d, is supposed to be: %d\n", sizeof(tempArr), tempArr->size);
            arrayRebuild(arr, tempArr->array, returnedByteCount);
            printf("Size of buffer atm: %d. Is supposed to be %d\n", sizeof(arr), arr->size);
        }
        else {
            printf("Filled the buffer.");
            break;
        }
    }
    fclose(inputFD); 
    arrayCompletion(arr);
    printf("Exited loop!\n");
    printf("Char count: %d \n", arr->size);
    printf("Size of array: %d", sizeof(arr->array));

    for (int i = 0; i < arr->size; i++) {
        printf("%c", arr->array[i]);
    }
    sig->ready_signal = 1; // indicates this child has finished its job
    return 1;
}

int readChars(char* tempBuffer, FILE* fdin) { // will return the number of bytes read.
    int index = 0;
    int currentChar;
    while (index < 15) { // words have hard limit of 15 chars
        currentChar = getc(fdin);
        if (currentChar == '\n' || currentChar == '\0') { // only terminators for a line/word
            break;
        }
        else {
            tempBuffer[index] = currentChar;
            index++;
        }
    }
    return index; // return number of chars (bytes) read.
}

void shortenArray(dynamic_array* arr_pointer, int index, char* tempBuffer) {
    // allocates memory for appropriate length array instead of 15 byte long one.
    arr_pointer->size = index;
    arr_pointer->array = (char*) calloc(arr_pointer->size, 1);
    for (int i = 0; i < arr_pointer->size; i++) {
        arr_pointer->array[i] = tempBuffer[i];
    }
}

void arrayRebuild(dynamic_array* arr_pointer, char* arr2, int index) {
    // arg1 is existing array, arg2 is new array, arg3 is length of new array.
    // appends arg1 and arg2 and replaces dynamic_array struct with new one.

    dynamic_array *newArray = arr_pointer;
    arr_pointer->wordCount++;
    int oldSize = arr_pointer->size;
    int newSize = oldSize + index + 1;
    arr_pointer->array = (char*) calloc(newSize, sizeof(char)); // callocs to new size.

    for (int i = 0; i < oldSize; i++) { // adds all of the old words
        arr_pointer->array[i] = (char) newArray->array[i];
    }

    int shift = oldSize;
    for (int i = 0; i < index; i++) { // adds the new word. For readability
        arr_pointer->array[i+shift] = (char) arr2[i];
    }
    arr_pointer->array[newSize] = ' '; // add a space at end because its in between words.
    arr_pointer->size = newSize;
}

void arrayCompletion(dynamic_array* arr) { // i don't know if i actually need this or not. depends on later implentation i suppose.
    dynamic_array* newArray = arr;
    arr->array = (char*) calloc(newArray->size + 1, 1); // increment by one to handle the null terminator

    for (int i = 0; i < arr->size; i++) {
        arr->array[i] = newArray->array[i]; // remaps the values to new array
    }
    arr->array[arr->size + 1] = '\0';
    arr->size ++;
}