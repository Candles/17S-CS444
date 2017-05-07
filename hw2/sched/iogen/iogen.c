#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFUALT_NUM_FILES 100

void createfiles(int totalitems){
    int i = 0; 
    int j = 0;
    int k = 0;
    FILE *f;
    char filename[10];

    for (; i < totalitems; i++){
        sprintf(filename, "%d.txt", i);
        
        f = fopen(filename, "w");
        
        for (j = 0; j < 10000; j++){
            fprintf(f, "%d", rand());
        }
        fclose(f);
    }

}

int main(int argc, char const *argv[]){
    int num = DEFUALT_NUM_FILES;

    if (argc <= 1){
        createfiles(num);
    }
    else {
        num = atoi(argv[1]);
        createfiles(num);
    }
    return 0;
}