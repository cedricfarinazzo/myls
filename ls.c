#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include "arg.h"
#include "scanner.h"

#define PRG_NAME "ls"
#define PRG_AUTHORS "Cédric Farinazzo<cedric.farinazzo@gmail.com>"
#define PRG_VERSION_MAJOR "0"
#define PRG_VERSION_MINOR "1"
#define PRG_VERSION_PATCH "0"

int version()
{
    printf("%s by %s  Version: %s.%s.%s\n", PRG_NAME, PRG_AUTHORS, PRG_VERSION_MAJOR, PRG_VERSION_MINOR, PRG_VERSION_PATCH);
    return EXIT_SUCCESS;
}

int help()
{
    printf("  Usage:  %s <options> <path> \n\n", PRG_NAME);
    printf(" path: path to file or directory\n\n"
    "   OPTION:\n\n"
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
    
    size_t nbtree = op.p->len == 0 ? 1 : op.p->len;
    struct entity *forest[nbtree];
    
    if (op.p->len == 0) {
        forest[0] = build_tree(".");
    } else {
        for (size_t i = 0; i < nbtree; ++i)
            forest[i] = build_tree(op.p->paths[i]);
    }

    for (size_t i = 0; i < nbtree; ++i)
        print_tree(forest[i], 0);
    
    for (size_t i = 0; i < nbtree; ++i)
        free_tree(forest[i]);
    
    clear_op(&op);
    return EXIT_SUCCESS;
}
