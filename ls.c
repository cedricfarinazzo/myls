#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

#define PRG_NAME "ls"
#define PRG_AUTHORS "Cédric Farinazzo<cedric.farinazzo@gmail.com>"
#define PRG_VERSION_MAJOR "0"
#define PRG_VERSION_MINOR "1"
#define PRG_VERSION_PATCH "0"

struct path {
    size_t len;
    char **paths;
} path;

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
    struct path *p;
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
    op->p = malloc(sizeof(struct path));
    op->p->len = 0;
    op->p->paths = malloc(sizeof(char*) * op->p->len);

    for(int i = 1; i < argc; i++)
    {
        char *arg = argv[i];

        if (strcmp("--nocolor", arg) == 0)
        {
            op->color = 0; continue;
        }
        if (strcmp("--help", arg) == 0)
        {
            op->help = 1; break;
        }
        if (strcmp("--version", arg) == 0)
        {
            op->version = 1; break;
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

int version()
{
    printf("%s by %s  Version: %s.%s.%s\n", PRG_NAME, PRG_AUTHORS, PRG_VERSION_MAJOR, PRG_VERSION_MINOR, PRG_VERSION_PATCH);
    return EXIT_SUCCESS;
}

int help()
{
    printf("  Usage:  %s <options> <path> \n\n", PRG_NAME);
    
    printf(" path: path to file or directory\n\n");

    printf("   OPTION:\n\n"
    " --version: diplay version\n"
    " --help: display help\n"
    " -a: display all\n"
    " -A: display all without . and ..\n"
    " -l: display in list\n"
    " --nocolor: disable color\n"
    " -s: display size\n"
    " -p: display permission\n"
    " -t: display time\n"
    " -R: recursive\n\n"
    );
    return version();
}

void clear_op(struct options *op)
{
    struct path *p = op->p;
    for (size_t i = 0; i < p->len; ++i)
        free(p->paths[i]);
    free(p);
}

int main(int argc, char *argv[])
{   
    struct options op;
    if (get_arg(&op, argc, argv) != 0)
        errx(1, "Couldn't read command line arguments");

    if (op.version)
    {
        clear_op(&op);
        return version();
    }

    if (op.help)
    {
        clear_op(&op);
        return help();
    }

    return EXIT_FAILURE;
}
