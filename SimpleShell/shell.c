/*
Submitter: Joya Debi
Group Member: Lucia Rhode
Prof Hakner
Ece357
Program 3 - Simple Shell Program (this didn't feel simple)
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>


int status; //should this be -1
char *commandArgs[BUFSIZ]; //no redirection
char *redir[BUFSIZ]; //redirection


int checkRedir(char *token){
    if(token[0] == '<'){
        return 1; //open filename and redirect stdin
    }
    else if(token[0] == '2' && token[1] =='>' && token[2] == '>'){ //&& (strcmp(&token[1],">")==0) && (strcmp(&token[2],">")==0)){
        return 5; //Open/Create/Append filename and redirect stderr
    }
    else if(token[0] == '>' && token[1] == '>'){// && (strcmp(&token[1],">")==0)){
        return 4; //Open/Create/Append filename and redirect stdout
    }
    else if(token[0] == '>'){
        return 2; //Open/Create/Truncate filename and redirect stdout
    }
    else if(token[0] == '2' && token[1] =='>'){// && (strcmp(&token[1],">")==0)){
        return 3; //Open/Create/Truncate filename and redirect stderr
    }
    else{
        return 0;
    }
}

int redirFile( char *pathname, int flags, mode_t mode, int fdNew){
    int fd;
    
    if ((fd=open(pathname, flags, mode)) < 0){
        fprintf(stderr, "ERROR: Could not open file %s: %s\n", pathname, strerror(errno));
        return -1;
    }
    if(dup2(fd, fdNew) < 0)
    {
        fprintf(stderr, "ERROR: dup2 failed for fd = %d: %s\n", fd, strerror(errno));
        return -1;
    }
    if(close(fd) < 0){
        fprintf(stderr, "ERROR: Could not close file %s: %s\n", pathname, strerror(errno));
        return -1;
    }
    return 0;
}
int doRedir(char **redir){
    char *pathname;
    int returnval;

    for(int i = 0; redir[i] != NULL; i++){
        switch(checkRedir(redir[i])){
            case 1: //open filename and redirect stdin
                pathname = &redir[i][1];
                returnval = redirFile(pathname, O_RDONLY, 0666, 0);
                break;
            case 2: //Open/Create/Truncate filename and redirect stdout
                pathname = &redir[i][1];
                returnval = redirFile(pathname, O_WRONLY|O_TRUNC|O_CREAT, 0666, 1);
                break;
            case 3: //Open/Create/Truncate filename and redirect stderr
                pathname = &redir[i][2];
                returnval = redirFile(pathname, O_WRONLY|O_TRUNC|O_CREAT, 0666, 2);
                break;
            case 4: //Open/Create/Append filename and redirect stdout
                pathname = &redir[i][2];
                returnval = redirFile(pathname, O_WRONLY|O_APPEND|O_CREAT, 0666, 1);
                break;
            case 5: //Open/Create/Append filename and redirect stderr
                pathname = &redir[i][3];
                returnval = redirFile(pathname, O_WRONLY|O_APPEND|O_CREAT, 0666, 2);
                break;
            default:
                return -1;
        }
        if(returnval == -1){
            return -1;
        }
    }
    return 0;
}

int checkCommand(char **commandArgs, char **redir){
    if((commandArgs[0] == NULL) || (strncmp(commandArgs[0],"#",1) == 0))
    {
        return 1;
    }
    else if(strncmp(commandArgs[0], "cd", 2) == 0)
    {
        return 2;//change directory
    }
    else if(strncmp(commandArgs[0], "pwd",3) == 0)
    {
        return 3;//print working directory
    }
    else if(strncmp(commandArgs[0], "exit",4) == 0)
    {
        return 4;//terminate shell
    }
    else{ //fork
        return 0;
        } 
}
int doCommand(char **commandArgs, char **redir, FILE *input){
    char cwd[BUFSIZ];
    pid_t pid, wpid;
    struct rusage rusage;
    struct timeval start, end;
    switch(checkCommand(commandArgs,redir)){
        case 1: //comment or nothing
            return 1;
            break;
        case 2: //change directory
            if(commandArgs[1]==NULL){
                commandArgs[1] = getenv("HOME");
            }
            if (chdir(commandArgs[1]) < 0){
                fprintf(stderr, "ERROR: failed to change current working directory: %s\n", strerror(errno));
                return -1;    
            }
            else{
                return 1;
            }
            break;
        case 3: //print working directory
            if((getcwd(cwd, sizeof(cwd))) == NULL){
                fprintf(stderr, "ERROR: Could not retrieve working directory: %s\n", strerror(errno));
            }
            else{
                printf("%s\n", cwd);
            }
            return 1;
            break;
        case 4://exit
            if(commandArgs[1]==NULL){
                exit(0);
            }
            else{
                exit(atoi(commandArgs[1]));
            }
        default:
            gettimeofday(&start, NULL);
            pid = fork();
            if(pid == 0){// we are in the child, exec
                if(doRedir(redir) < 0){
                    fprintf(stderr, "ERROR: IO redirection failed\n");
                    exit(EXIT_FAILURE);
                }
                if(input != stdin){
                    fclose(input);
                }
                if(execvp(commandArgs[0], commandArgs) == -1){
                    fprintf(stderr, "ERROR: Could not execute command %s: %s\n", commandArgs[0], strerror(errno));
                }
                exit(EXIT_FAILURE);
            }
            else if(pid == -1){// fork failed
                fprintf(stderr, "ERROR: fork failed: %s\n", strerror(errno));
                break;
            }
            else{//we are in the parent, wait for child to terminate
                wpid = wait3(&status, 0, &rusage);
                if(wpid < 0){
                    fprintf(stderr, "ERROR: Child process error: %s", strerror(errno));
                    break;
                }
                else{
                    gettimeofday(&end, NULL);
                    fprintf(stderr, "Child process %d exited normally with return value %i\n", getpid(), WEXITSTATUS(status));
                    fprintf(stderr, "Real: %ld.%06us, User: %ld.%06us Sys: %ld.%06us\n", (end.tv_sec-start.tv_sec), (end.tv_usec-start.tv_usec), rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec, rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec);
                }
                break;
            }
        }
    return 1;
}

int parseLine(char* line, FILE* input){
    char *delim = " \t\n";
    char *token;
    int i = 0, j = 0;
    int commandCase;
    token = strtok(line, delim);
    while(token != NULL){
        if(checkRedir(token) == 0){
            commandArgs[i] = token;
            i++;
        }
        else if(checkRedir(token) != 0){
            redir[j] = token;
            j++;
        }
        token = strtok(NULL, delim);
    }

    
    commandArgs[i] = NULL; 
    redir[j] = NULL;
    status = doCommand(commandArgs, redir, input);
    return 0;
}

int main(int argc, char *argv[]){
    FILE *input;
    
    if(argc == 1){ //no file specified, make input stdin
        input = stdin;
    }
    else{
        if((input = fopen(argv[1], "r")) == NULL){
            fprintf(stderr, "ERROR: Could not open file %s: %s\n", argv[1], strerror(errno));
            return -1;
        }
    }
    
    int running = 1;//while loop must execute once
    size_t n = 0;
    while(running){
        char* lineptr = NULL; //set to NULL so getline will allocate buffer to store line
        if(getline(&lineptr, &n, input) != -1){//if line sucessfully read
            if(strcmp(lineptr, "\n") == 0 || strcmp(lineptr, "#") == 0){
                fprintf(stderr, "$ ");
            }
            parseLine(lineptr, input); //parse line for arguments
            memset(commandArgs, 0, BUFSIZ); //clear buffer
        }
        else if (feof(input) == 0) {
            fprintf(stderr, "ERROR: Could not read command from stdin: %s\n", strerror(errno));
        }
        else{
            break;
        }
    } 
    printf("\n");
    exit(status);
    return 0;
}