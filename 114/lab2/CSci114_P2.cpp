// To run this program you need to compile it using c++11

// Place a data.in file into the same directory as the executable
// then run the executable and a data.out file will be made in place.


// The relative addressing is super fragile. I know Windows changes your
// working directory when you elevate permissions so this obviously wouldn't
// work there, but this is written to spec and no one wants to have to type
// or copy/paste file addresses anyway.

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
using namespace std;

int A_to_B[2], B_to_A[2];

int main() {
    int status;
    int blockSize = 0x1000;
    streamsize readSize;

    if (pipe(A_to_B)) {
        printf("UNABLE TO OPEN PIPE");
        return 0;
    }

    if (!fork()) {
        printf("In process A\n");
        close(A_to_B[0]);
        ifstream inFile ("data.in", ios::in | ios::binary);
        char * memBlock = new char[blockSize];
        while(inFile.good()) {
            strcpy(memBlock, "good");
            if (write(A_to_B[1], memBlock, 4) != 4) {
                printf("ERROR WRITING TO PIPE. FILE NOT COMPLETE\n");
                break;
            }
            inFile.read(memBlock, blockSize);
            readSize = inFile.gcount();
            if (write(A_to_B[1], memBlock, blockSize) != blockSize) {
                printf("ERROR WRITING TO PIPE. FILE NOT COMPLETE\n");
                break;
            }
            if (write(A_to_B[1], &readSize, sizeof(streamsize)) != sizeof(streamsize)) {
                printf("ERROR WRITING TO PIPE. FILE NOT COMPLETE\n");
                break;
            }
            printf("READ: %ld bytes\n", readSize);
        }
        strcpy(memBlock, "done");
        if (write(A_to_B[1], memBlock, 4) != 4)
            printf("ERROR WRITING TO PIPE. FORK TERMINATE FAILED.");
        else
            printf("COMPLETED: File read\n");
        delete memBlock;
        return 0;
    }
    if (!fork()) {
        printf("In process B\n");
        ofstream outFile("data.out", ios::out | ios::binary);
        char * memBlock = new char[blockSize];
        char * pipeStatus = new char[4];
        while (true) {
            int nbytes = read(A_to_B[0], pipeStatus, 4);
            if (nbytes > 0) {
                if (strcmp(pipeStatus, "done") == 0) {
                    printf("COMPLETED: File write\n");
                    delete memBlock;
                    delete pipeStatus;
                    return 0;
                }
                while (true) {
                    nbytes = read(A_to_B[0], memBlock, blockSize);
                    if (nbytes > 0) {
                        while (true) {
                            if (read(A_to_B[0], &readSize, sizeof(streamsize)) > 0) {
                                outFile.write(memBlock, readSize);
                                printf("WROTE: %ld bytes\n", readSize);
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    wait(&status);
    wait(&status);
    printf("END\n");
    return 0;
}
