#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define BUFFER_SIZE 1000

int main(int argc, char* argv[]) {
    char buffer[BUFFER_SIZE];
    char inpDict[26];
    char encryptDict[26];

    strcpy(buffer, argv[1]);
    for (int i = 2; i < argc; i++) {
        strcat(buffer, " ");
        strcat(buffer, argv[i]);
    }
    strcat(buffer, "\0");

    int len = strlen(buffer);
    // printf("Test %d: %s\n", len, buffer);
    
    //Get input dict
    int exists = 0;
    int dictPos = 0;
    for (int i = 0; i < len; i++) {
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

    //Check input dict
    // printf("blegh: %s\n", inpDict);

    //Randomise
    char curr;
    int randNum;
    int dictLen = strlen(inpDict);
    int tempPos = 0;
    for (int i = 0; i < dictLen; i++) {
        randNum = rand() % dictLen;
        curr = inpDict[randNum];
        for (int j = 0; j < dictLen; j++) {
            if (encryptDict[j] == curr) {
                curr = inpDict[tempPos];
                tempPos++;
                j = -1;
            }
        }
        encryptDict[i] = curr;
    }
    encryptDict[dictPos] = '\0';

    // printf("encrypt: %s\n", encryptDict);

    //Replace chars in input string with randomised set
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            if (buffer[i] == inpDict[j]) {
                buffer[i] = encryptDict[j];
                break;
            }
        }
    }
    // printf("encrypted string: %s\n", buffer);

    FILE *fp = fopen("ciphertext.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "%s\n", buffer);
    }

    fclose(fp);
    return 0;
}