#include <stdio.h>
#include <stdlib.h>

#define DEFUALT_NUM_FILES 10

void createfiles(int totalitems){
    int i = 0; 
    int j = 0;
    int k = 0;
    FILE *f;
    char filename[10];

    // First pass
    for (; i < totalitems; i++){
        sprintf(filename, "%d.txt", i);
        f = fopen(filename, "w");
        
        for (j = 0; j < 10000; j++){
            fprintf(f, "%d", rand());
        }
        fclose(f);
    }

    // Second pass - not sure if necessary
    for (i = 0; i < (totalitems * 2); i++){
        sprintf(filename, "%d.txt", (rand() % totalitems));
        f = fopen(filename, "a");
        for (j = 0; j < 1000; j++){
            fprintf(f, "%d", rand());
        }
        fclose(f);
    }

}

int main(int argc, char const *argv[]){
    int num = DEFUALT_NUM_FILES;

    if (argc <= 1){
        createfiles(DEFUALT_NUM_FILES);
    }
    else {
        num = atoi(argv[1]);
        createfiles(num);
    }
    return 0;
}