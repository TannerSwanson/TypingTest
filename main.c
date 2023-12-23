#include <stdio.h>

typedef struct {
    int size;
    char* array;
} dynamic_array;

int newLine() {
    // This works, however we have an FD problem. 
    // This is Child b (reads from a, outs to user)
    // Child a (reads from wordlist2.txt, outs to b)
    // Child c validates user input (in from user, out to user)

    dynamic_array* arr;
    int fd[2];
    int bufSize = 300; // bufferSize
    while (bufSize > 0) { // we will use this to fill our arr until it is max capacity.
        char* wordBuffer[15];
        int returnedByteCount = read(fd[0], &wordBuffer, 15); // read from another thread which had its FD's remapped. (TODO)
        if (bufSize - returnedByteCount >= 0) {
            if (returnedByteCount <= 0) { 
                // Failed operation
                printf("Read Failure.");
                return -1;
            }
            else {
                arrayRebuild(&arr, &wordBuffer);
            }
            return 0;
        }
        else {
            printf("Buffer overflow prevented.");
            bufSize = 0; // used to exit while loop.
        }
    }
    // now we can start to display all of the information.
    for (int i = 0; i < arr->size; i++) {
        printf("%c", arr->array[i]);
        printf("\n");
    }
}


int readWords() {
    // TODO: Remember how to remap fd's. Also create the shared struct that the 3 will use to communicate. 
    // Currently we need a signal_from_b varaible (could rename), but needs to be signal from b (newLine) to a (readWords)
    // that it is ready to receive words.
    int fd[2];
    int signal_from_b = 1; // temp, until errors stop.
    while (1) {
        int break_Flag = 0;
        if (signal_from_b) {
            char* wordBuffer[15];
            int returnedByteCount = read(fd[0], &wordBuffer, 15);
            for (int i = 0; i < 15; i++) {
                if (wordBuffer[i] == '\0' || wordBuffer[i] == '\n') {
                    write(fd[1], &wordBuffer, (i-1)); //if we reached end, stop and write from here.
                    break_Flag = 1;
                    signal_from_b = 0;
                    break;
                }
            }
            if (break_Flag == 0) {
                write(fd[1], &wordBuffer, 15);
                signal_from_b = 0;
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