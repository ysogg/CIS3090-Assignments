#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mpi.h>

#define BUFFER_SIZE 10000
char msgBuffer[BUFFER_SIZE] = "";


void decryptFunc(char input[BUFFER_SIZE], char inpDict[26], char decryptDict[26], char sysDictionary[50], int inputSize, int dictLen) {
    char hold[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    strcpy(hold, "");
    strcpy(output, "");

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
    hold[inputSize] = '\0';
    strcpy(output, hold);
    
    int grepStatus = 0;
    char *tok = strtok(hold, " ");
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
        if (strcmp(msgBuffer, "") == 0) {
            strcpy(msgBuffer, output);
            strcat(msgBuffer, "\0");
        } else {
            strcat(msgBuffer, "|");
            strcat(msgBuffer, output);
            strcat(msgBuffer, "\0");
        }
    }
}

void swapChars(char dict[26], int apos, int bpos);

//expecting dict of n-1 chars where first char is substituted in after
//dictLen should reflect this
void generatePerms(char input[26], int size, char first, char inpDict[26], char sysDictionary[50], char buffer[BUFFER_SIZE], int inputSize, int dictLen) {
    if (size == 1) {
        char perm[26];
        strcpy(perm, "");
        perm[0] = first;
        perm[1] = '\0';
        strcat(perm, input);
        strcat(perm, "\0");

        char bufferCpy[BUFFER_SIZE];
        strcpy(bufferCpy, buffer);

        decryptFunc(bufferCpy, inpDict, perm, sysDictionary, inputSize, dictLen);
        return;
    } else {
        for (int i = 0; i < size; i++) {
            generatePerms(input, size - 1, first, inpDict, sysDictionary, buffer, inputSize, dictLen);
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
// RUN: mpiexec -n 4 ./decrypt

int main(int argc, char* argv[]) {
    //MPI SECTION
    int comm_sz; /* Number of processes    */ 
    int my_rank; /* My process rank        */
    char msgStr[BUFFER_SIZE];

    char input[BUFFER_SIZE] = "";
    char inpDict[26] = "";
    char decryptDict[26] = "";
    int inputSize = 0;
    int dictLen = 0;
    char sysDictionary[50] = "";
    
    /* Start up MPI */
    MPI_Init(NULL, NULL); 

    /* Get the number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

    /* Get my rank among all the processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        char filename[100];
        // char sysDictionary[100];

        strcpy(filename, argv[1]);
        strcpy(sysDictionary, argv[2]);

        FILE *fp = fopen(filename, "r");
        if (fp != NULL) {
            char ch;
            int count = 0;
            while ((ch = fgetc(fp)) != EOF) {
                input[count] = ch;
                count++;
            }
            if (input[count-1] == '\n') {
                input[count-1] = '\0';
            } else {
                input[count] = '\0';
            }
            
        } else {
            printf("Could not open file\n");
            MPI_Abort(MPI_COMM_WORLD, 0);
            return 1;
        }
        fclose(fp);
        
        // printf("blegh: %s\n", input);
        inputSize = strlen(input);

        //Get input dict (this is the jumbled version from ciphertext)
        int exists = 0;
        int dictPos = 0;
        for (int i = 0; i < inputSize; i++) {
            if (tolower(input[i]) >= 97 && tolower(input[i]) <= 122) {
                for (int j = 0; j < 26; j++) {
                    if (inpDict[j] == tolower(input[i])) {
                        exists = 1;
                    }   
                }   

                if (exists != 1) {
                    inpDict[dictPos] = tolower(input[i]);
                    dictPos++;
                }
                exists = 0;
            }
        }
        inpDict[dictPos] = '\0';
        strcpy(decryptDict, inpDict);
        // printf("orig decrypt: %s\n", decryptDict);
        dictLen = strlen(inpDict);
    }

    
    MPI_Bcast(input, BUFFER_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(inpDict, 26, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(sysDictionary, 50, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (my_rank != 0) {

        inputSize = strlen(input);
        dictLen = strlen(inpDict);
        strcpy(decryptDict, inpDict);

        //Generate permutations at rank
        char cpyDict[26];
        int extra = 0;
        if (dictLen % comm_sz == 0) {
            int mult = dictLen / comm_sz;
            //Checking how many
            for (int i = 0; i < mult; i++) {
                swapChars(decryptDict, 0, i + (mult * my_rank));
                
                strncpy(cpyDict, decryptDict + 1, dictLen-1);
                cpyDict[dictLen-1] = '\0';

                generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
            }
        } else if (dictLen > comm_sz) {
            extra = dictLen % comm_sz;
            int mult = (dictLen-extra) / comm_sz;

            if (my_rank < extra) {
                for (int i = 0; i < mult + 1; i ++) {
                    swapChars(decryptDict, 0, i + (mult * my_rank) + my_rank);

                    strncpy(cpyDict, decryptDict + 1, dictLen-1);
                    cpyDict[dictLen-1] = '\0';

                    generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
                }
            } else {
                for (int i = 0; i < mult; i ++) {
                    swapChars(decryptDict, 0, i + (mult * my_rank) + extra);

                    strncpy(cpyDict, decryptDict + 1, dictLen-1);
                    cpyDict[dictLen-1] = '\0';

                    generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
                }
            }
        } else if (dictLen < comm_sz) {
            if (my_rank < dictLen) {
                swapChars(decryptDict, 0, my_rank);

                strncpy(cpyDict, decryptDict + 1, dictLen-1);
                cpyDict[dictLen-1] = '\0';

                generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
            }
        }
        // printf("sending from %d\n", my_rank);
        MPI_Send(msgBuffer, strlen(msgBuffer)+1, MPI_CHAR, 0, 5, MPI_COMM_WORLD);         
        
    } else {
        //////RANK 0////////

        //handler process
        char cpyDict[26];
        int extra = 0;
        if (dictLen % comm_sz == 0) {
            int mult = dictLen / comm_sz;
            //Checking how many
            for (int i = 0; i < mult; i++) {
                swapChars(decryptDict, 0, i + (mult * my_rank));
                
                strncpy(cpyDict, decryptDict + 1, dictLen-1);
                cpyDict[dictLen-1] = '\0';

                generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
            }
        } else if (dictLen > comm_sz) {
            extra = dictLen % comm_sz;
            int mult = (dictLen-extra) / comm_sz;

            if (my_rank < extra) {
                for (int i = 0; i < mult + 1; i ++) {
                    swapChars(decryptDict, 0, i + (mult * my_rank) + my_rank);

                    strncpy(cpyDict, decryptDict + 1, dictLen-1);
                    cpyDict[dictLen-1] = '\0';

                    generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
                }
            } else {
                for (int i = 0; i < mult; i ++) {
                    swapChars(decryptDict, 0, i + (mult * my_rank) + extra);
                    

                    strncpy(cpyDict, decryptDict + 1, dictLen-1);
                    cpyDict[dictLen-1] = '\0';

                    generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
                }
            }        
        } else if (dictLen < comm_sz) {
            if (my_rank < dictLen - (comm_sz-dictLen)) {
                swapChars(decryptDict, 0, my_rank);

                strncpy(cpyDict, decryptDict + 1, dictLen-1);
                cpyDict[dictLen-1] = '\0';

                generatePerms(cpyDict, dictLen-1, decryptDict[0], inpDict, sysDictionary, input, inputSize, dictLen);
            }
        }

        
        char* rank_zero_tok;
        //output rank 0 matches if any
        if (strcmp(msgBuffer, "") != 0) {
               rank_zero_tok = strtok(msgBuffer, "|"); 
               while (rank_zero_tok != NULL) {
                    printf("%s\n", rank_zero_tok);
                    rank_zero_tok = strtok(NULL, "|");
               }
            }

        for (int q = 1; q < comm_sz; q++) {
            char* tok;
            /* Receive message from process q */
            MPI_Recv(msgStr, BUFFER_SIZE, MPI_CHAR, q, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            /* Print message from process q */
            if (strcmp(msgStr, "") != 0) {
               tok = strtok(msgStr, "|"); 
               while (tok != NULL) {
                    printf("%s\n", tok);
                    tok = strtok(NULL, "|");
               }
            }
        }
    }

    /* Shut down MPI */
    MPI_Finalize(); 

    return 0;
}