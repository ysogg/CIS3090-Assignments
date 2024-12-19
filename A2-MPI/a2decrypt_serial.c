#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1000
char buffer[BUFFER_SIZE];
int inputSize = 0;
int dictLen = 0;
int permCount = 0;


void decryptFunc(char input[BUFFER_SIZE], char inpDict[26], char decryptDict[26], char sysDictionary[100]) {
    char hold[inputSize];
    char output[inputSize];
    for (int i = 0; i < inputSize; i++) {
        for (int j = 0; j < dictLen; j++) {
            if (input[i] == ' ') {
                hold[i] = ' ';
                break;
            } else if (input[i] == inpDict[j]) {
                hold[i] = decryptDict[j];
                break;
            }
        }
    }
    strcpy(output, hold);
    
    int grepStatus = 0;
    char *tok = strtok(hold, " ");
    // printf("%s\n", tok);
    while (tok != NULL) {
        char *cmd[] = {"grep", "-i", "-x", "-q", tok, sysDictionary, NULL};

        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Failed to fork :(\n");
        } else if (pid == 0) {
            // printf("child\n");
            if (execvp(cmd[0], cmd) < 0) {
                fprintf(stderr, "exec failed\n");
            }
            // printf("t: %s\n", tok);
            
        } else {
            int status;
            waitpid(pid, &status, 0);

            if (WEXITSTATUS(status) != 0) {
                grepStatus = 1;
            }
        }

        if (grepStatus == 1) {
            break;
        }
        
        tok = strtok(NULL, " ");
    }

    if (grepStatus != 1) {
        printf("%s\n", output);
    }
}

void swapChars(char dict[26], int apos, int bpos);

//expecting dict of n-1 chars where first char is substituted in after
//dictLen should reflect this
void generatePerms(char input[26], int size, char first, char inpDict[26], char sysDictionary[100]) {
    if (size == 1) {
        char perm[30];
        //add to permutation array
        strcpy(perm, "");
        perm[0] = first;
        perm[1] = '\0';
        strcat(perm, input);

        char bufferCpy[BUFFER_SIZE];
        strcpy(bufferCpy, buffer);
        decryptFunc(bufferCpy, inpDict, perm, sysDictionary);
        return;
    } else {
        for (int i = 0; i < size; i++) {
            generatePerms(input, size - 1, first, inpDict, sysDictionary);
            if (size % 2 == 1) {
                swapChars(input, 0, size - 1);
            } else {
                swapChars(input, i, size - 1);
            }
        }
    }
}

void swapChars(char dict[26], int apos, int bpos) {
    char tmp[26];
    strcpy(tmp, dict);

    dict[apos] = dict[bpos];
    dict[bpos] = tmp[apos]; 
}

int main(int argc, char* argv[]) {
    // char buffer[BUFFER_SIZE];
    char inpDict[26];
    char decryptDict[26];

    char filename[100];
    char sysDictionary[100];

    strcpy(filename, argv[1]);
    strcpy(sysDictionary, argv[2]);

    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        char ch;
        int count = 0;
        while ((ch = fgetc(fp)) != EOF) {
            buffer[count] = ch;
            count++;
        }
            if (buffer[count-1] == '\n') {
                buffer[count-1] = '\0';
            } else {
                buffer[count] = '\0';
            }
    }
    fclose(fp);
    // printf("buffy: %s\n", buffer);
    inputSize = strlen(buffer);

    //Get input dict (this is the jumbled version from ciphertext)
    int exists = 0;
    int dictPos = 0;
    for (int i = 0; i < inputSize; i++) {
        if (tolower(buffer[i]) >= 97 && tolower(buffer[i]) <= 122) {
            for (int j = 0; j < 26; j++) {
                if (inpDict[j] == tolower(buffer[i])) {
                    exists = 1;
                }   
            }   

            if (exists != 1) {
                inpDict[dictPos] = tolower(buffer[i]);
                dictPos++;
            }
            exists = 0;
        }
    }
    inpDict[dictPos] = '\0';


    //Serial decrypt
    //for each letter inpDict, make a decryptDict
    strcpy(decryptDict, inpDict);
    // printf("b4: %s\n", decryptDict);
    dictLen = strlen(inpDict);


    //Generate all permutations
    char cpyDict[26];
    for (int i = 0; i < dictLen; i++) {
        //at start of each swap first char with loop index
        swapChars(decryptDict, 0, i);
        strncpy(cpyDict, decryptDict + 1, dictLen-1);
        cpyDict[dictLen-1] = '\0';

        //bah passing everything through here
        generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary);
    }

    return 0;
}