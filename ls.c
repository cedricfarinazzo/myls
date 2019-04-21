#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct options {
    int all, list, color, sorted, perm, link, time;
} options;


void get_arg(struct options *op, int argc, char *argv[])
{
    for(size_t i = 0; i < argc; i++)
        printf("%s\n", argv[i]);
}


int main(int argc, char *argv[])
{   
    struct options op;
    get_arg(&op, argc, argv);
    
    return EXIT_SUCCESS;
}
