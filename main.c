#include <stdio.h>

typedef struct {
    // Utilized by newLine and validate functions.
    int size;
    char* array;
} dynamic_array;

typedef struct {
    // shared struct to tell readWords to send to newLine. May be extended for Child c action.
    int signal_from_b;
} signals;

int newLine(dynamic_array* arr, signals* sig) {
    // This works, however we have an FD problem. 
    // This is Child b (reads from a, outs to user)
    // Child a (reads from wordlist2.txt, outs to b)
    // Child c validates user input (in from user, out to user)

    int fd[2]; // need to actually map the FD's. 
    int bufSize = 300; // bufferSize
    char* wordBuffer[15] = {0};
    while (bufSize > 0) { // we will use this to fill our arr until it is max capacity.
        sig->signal_from_b = 1;
        int returnedByteCount = read(fd[0], &wordBuffer, 15);
        if (bufSize - returnedByteCount >= 0) {
            if (returnedByteCount <= 0) { 
                // Failed operation
                printf("Read Failure.");
                return -1;
            }
            else {
                arrayRebuild(&arr, &wordBuffer);
            }
            memset(wordBuffer, 0, 15); //resets wordBuffer to all 0's
        }
        else {
            printf("Buffer overflow prevented.");
            bufSize = 0; // used to exit while loop.
            memset(wordBuffer, 0, 15); //resets wordBuffer to all 0's
        }
    }
    sig->signal_from_b = 0; // disable signal.

    // now we can start to display all of the information.
    for (int i = 0; i < arr->size; i++) {
        printf("%c", arr->array[i]);
        printf("\n");
    }
}


int readWords(signals* sig) {
    // TODO: Remember how to remap fd's.
    
    int fd[2];
    while (1) {
        int break_Flag = 0;
        if (sig->signal_from_b == 1) {
            char* wordBuffer[15];
            int returnedByteCount = read(fd[0], &wordBuffer, 15);
            for (int i = 0; i < 15; i++) {
                if (wordBuffer[i] == '\0' || wordBuffer[i] == '\n') {
                    write(fd[1], &wordBuffer, (i-1)); //if we reached end, stop and write from here.
                    break_Flag = 1;
                    sig->signal_from_b = 0;
                    break;
                }
            }
            if (break_Flag == 0) {
                write(fd[1], &wordBuffer, 15);
                sig->signal_from_b = 0;
            }
        }
    }
    return -1;
}

void arrayRebuild(dynamic_array* arr_pointer, char* arr2) {
    dynamic_array *newArray;
    int newSize = sizeof(arr_pointer) + sizeof(arr2);
    newArray->array = (dynamic_array*) malloc(newSize * sizeof(char)); 

    // These 2 loops write all of the existing and new entries to the array.
    for (int i = 0; i < sizeof(arr_pointer); i++) {
        newArray->array[i] = (char) arr_pointer->array[i];
    }
    for (int i = arr_pointer->size; i < arr_pointer->size + sizeof(arr2); i++) {
        newArray->array[i] = (char) arr2[i];
    }
    newArray->size = newSize;
    newArray->array[newSize + 1] = '\0'; //null terminator to finish it off. Probably will help later.
}