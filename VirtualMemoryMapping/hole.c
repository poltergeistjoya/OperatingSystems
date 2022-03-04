#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){
        //open or make a file
        int fd;
        fd = open("./file.txt", O_RDWR|O_CREAT|O_TRUNC, 0666);
        if(fd < 0){
                fprintf(stderr, "open failed\n");
                        exit(EXIT_FAILURE);
        }
        fprintf(stderr, "file.txt opened!\n");
        //write 4100 bytes to file, 4 bytes more than page size
        int i;
        for(i = 0; i <= 4100; i++){
                if(write(fd, "A", 1) != 1){
                        exit(EXIT_FAILURE);
                }
        }
        fprintf(stderr, "4100 bytes written to file.txt\n");
        char *addr = mmap(NULL, 2*4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if(addr == MAP_FAILED){
                fprintf(stderr, "mmap failed\n");
                exit(EXIT_FAILURE);
        }
        //read memory mapped region and make sure not written to areas are NULL
        for(i= 4101; i<8192; i++){
            if(addr[i]== (char)NULL){
                continue; 
            }
            else{
                exit(EXIT_FAILURE);
            }
        }
        fprintf(stderr, "Bytes 4101 to 8192 in mmaped region are null\n");
        addr[4102] = 'X';
        fprintf(stderr, "X written to byte 4102 of mmaped region\n");
        //lseek and write one byte
        lseek(fd, 4500 , SEEK_SET);
        write(fd, "a", 1);
        fprintf(stderr, "wrote 1 byte to offset 4500 in file.txt, thus creating a hole in the file\n");
        //now lseek back and see if the x written in mmaped region is readable
        lseek(fd, 4102, SEEK_SET);
        char buf[1];
        int buf_size = sizeof(buf);
        if(read(fd, buf, buf_size) == -1){
                fprintf(stderr, "read failed\n");
                exit(EXIT_FAILURE);
        }
        if(buf[0] != 'X'){
            return 1; 
        }
        else if(buf[0] =='X'){
            fprintf(stderr, "Value written in mmapped region is seen in file as: %c\n", buf[0]);
            fprintf(stderr, "Writing to hole in file via mmapped region is visible in file via read!\n");
            return 0;
        }    
}