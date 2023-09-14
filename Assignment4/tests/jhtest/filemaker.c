#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){

    //make the file name
    char* filename;
    char* filename2;
    char* filename3;

    //make the file
    FILE *fp;
    FILE *fp2;
    FILE *fp3;

    for(int i = 70; i<80;i++){
        //make the file name
        filename = (char*)malloc(sizeof(char)*20);
        filename2 = (char*)malloc(sizeof(char)*20);
        filename3 = (char*)malloc(sizeof(char)*20);

        if (i<10){
            sprintf(filename, "jhtest0%d.txt", i);
            sprintf(filename2, "jhtest0%d.stdout", i);
            sprintf(filename3, "jhtest0%d.stderr", i);
        }
        else{
            sprintf(filename, "jhtest%d.txt", i);
            sprintf(filename2, "jhtest%d.stdout", i);
            sprintf(filename3, "jhtest%d.stderr", i);
        }

        //make the file
        fp = fopen(filename, "w");
        fp2 = fopen(filename2, "w");
        fp3 = fopen(filename3, "w");

    }

    

    
}