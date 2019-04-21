#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

#define PRG_NAME "ls"
#define PRG_AUTHORS "Cédric Farinazzo<cedric.farinazzo@gmail.com>"
#define PRG_VERSION_MAJOR 0
#define PRG_VERSION_MINOR 1
#define PRG_VERSION_PATCH 0

struct options {
    int version; //(0) / --version: diplay version
    int help;  // (0) / --help: display help
    int all;   // (0) / -a(1): all / -A(2): all without . and ..
    int list;  // (0) / -l(1): display in list
    int color; // (1) / --nocolor(0): disable color
    int size;  // (0) / -s(1): display size
    int perm;  // (0) / -p(1): display permission
    int time;  // (0) / -t(1): display time
    int rec;   // (0) / -R(1): recursive
} options;


int get_arg(struct options *op, int argc, char *argv[])
{
    op->version = 0;
    op->help = 0;
    op->all = 0;
    op->list = 0;
    op->color = 1;
    op->size = 0;
    op->perm = 0;
    op->time = 0;
    op->rec = 0;
    
    for(int i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        
        if (strcmp("--nocolor", arg) == 0)
        {
            op->color = 0;
            continue;
        }
        if (strcmp("--help", arg) == 0)
        {
            op->help = 1;
            break;
        }
        if (strcmp("--version", arg) == 0)
        {
            op->version = 1;
            break;
        }

        for (size_t i = 0; arg[i] != 0; ++i)
        {
            switch (arg[i]) {
                
                case 'a':
                    op->all = 1;
                    break;

                case 'A':
                    op->all = 2;
                    break;

                case 'l':
                    op->list = 1;
                    break;

                case 's':
                    op->size = 1;
                    break;

                case 'p':
                    op->perm = 1;
                    break;

                case 't':
                    op->time = 1;
                    break;
                
                case 'R':
                    op->rec = 1;
                    break;

                default:
                    break;
            }
        }
    }

    return 0;
}


int main(int argc, char *argv[])
{   
    struct options op;
    if (get_arg(&op, argc, argv) != 0)
        errx(1, "Couldn't read command line arguments");
    
    return EXIT_SUCCESS;
}
