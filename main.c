// we can use read for handling the cmd stuff if thats what we want to do (cmd line based typing test)
// read(int fd, void *buf, size_t count); and then also write to show the contents that we are comparing it to.
// could print say 100 words and then the user has to type them in one response in the terminal 
// buffer size??? count?? idk.


#include <stdio.h>

int main() {
    // TODO: Handle the words that we will be displaying. Can create a thread/fork to send them back, or could reroute IO
    // will need to do char arrays for words and word lengths, should seperate them by space marks.
    // buffer until char is at a certain size long, could be 300 chars (1-1 on byte length to chars).
    // will need to read char, (check if it is a backspace and handle that if so), otherwise compare to the char at that point.
    
    // this code works to read info and then spit it back out at me:
    char input[12];
    for (int i = 0; i < 12; i += 2) {
        // This reads a single char as input. 
        // It skips every other since the input is char + \0 (null terminator) to make input 2 bytes long
        scanf("%c", &input[i]);
        printf("%c", input[i]);
        printf("\n");
    }
    return 0;
}