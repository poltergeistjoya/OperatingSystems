#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

int main(int argc, char *argv[]) {

    int opt, fdin, fdout, firstInd, n, mod, RsysCount, WsysCount, byteCount, totalByte;
    int bufSize = 4096;
    int numInputs = 0;
    bool noInput;
    char *outFile;
    char *inFile;
    
    mod = 0;
    outFile = "";
    noInput = false;

    while ((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
            case 'o':
                outFile = optarg;
                break;
            default: 
                printf("ERROR: flag -o must be followed by an argument.");
                return -1;
        } 
    }
    
    char buff[bufSize];

    if (strcmp(outFile, "") != 0) { //if there is no outfile specified
        fdout=open(outFile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (fdout < 0) {
            fprintf(stderr, "ERROR: Issue opening output file %s for writing: %s\n", outFile, strerror(errno));
            return -1;
        }
    } else {
        fdout=1;
    }
    
    if (optind < argc) {
        firstInd = optind;
        while (optind < argc) {
            numInputs++;
            optind++;
        }
        optind = firstInd;
    } else {
        noInput = true;
        numInputs = 1;
    }

    for (int i = 0; i < numInputs; i++) {
        if (noInput == true) { //if no inputs, same condition as infile = "-"" : stdin->stdout
            inFile = "-";
        } else {
            inFile = argv[optind+i];
        }
    
        if ((strcmp(inFile, "-")) == 0) { //condition to take stdin when infile = "-"
            fdin = 0;
            inFile = "stdin";
        } else if ((fdin = open(inFile, O_RDONLY)) < 0) {
            fprintf(stderr, "ERROR: Issue opening input file %s for reading: %s\n", inFile, strerror(errno)); 
            return -1;
        }
        
        while ((n=read(fdin, buff, sizeof(buff))) != 0) { 
            RsysCount++; 
            int c;
                for (int j = 0; j<byteCount; j++) {
                    if((buff[j]>=127) | (buff[j]>0 & buff[j]<9) | (buff[j]>13 & buff[j]<32)) {
                    fprintf(stderr, "ERROR: %s file is binary: %s\n", inFile, strerror(errno));
                    break;
                }
            }
            if (n < 0) {
                fprintf(stderr, "ERROR: Issue reading input file %s: %s\n", inFile, strerror(errno));
                return -1;
            } else {
                while (mod < n) {
                    WsysCount++;
                    if ((mod = write(fdout, buff, n)) < 0) {
                        fprintf(stderr, "ERROR: Issue writing to output file %s: %s\n", outFile, strerror(errno));
                        return -1;
                    }
                    totalByte = totalByte + n;
                    n -= mod;
                    mod = 0;
                }
            }
        }

        if (fdin != 0) {
            if (close(fdin) < 0) {
                fprintf(stderr, "ERROR: Issue closing input file %s: %s\n", inFile, strerror(errno));
                return -1;
            }
        }
    }

    if (fdout != 1) {
        if (close(fdout) < 0) {
            fprintf(stderr, "ERROR: Issue closing output file %s: %s\n", outFile, strerror(errno));
            return -1;
        }
    }
    fprintf(stderr, "Total Bytes Transferred: %d\n",totalByte);
    fprintf(stderr, "Total Read System Calls: %d\n",RsysCount);
    fprintf(stderr, "Total Write System Calls: %d\n",WsysCount);
    return 0;
}