#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arg.h"

int get_arg(struct options *op, int argc, char *argv[])
{
    op->version = op->help = op->all = op->list = op->size = op->perm = op->time = op->rec = 0;
    op->color = 1;
    op->p = malloc(sizeof(struct path));
    op->p->len = 0;
    op->p->paths = malloc(sizeof(char*) * op->p->len);

    for(int i = 1; i < argc; i++)
    {
        if (strcmp("--nocolor", argv[i]) == 0)
        { op->color = 0; continue; }
        if (strcmp("--help", argv[i]) == 0)
        { op->help = 1; break; }
        if (strcmp("--version", argv[i]) == 0)
        { op->version = 1; break; }
        for (size_t j = 0; argv[i][j] != 0 && argv[i][0] == '-'; ++j)
        {
            switch (argv[i][j]) {
                case 'a':
                    op->all = 1; break;
                case 'A':
                    op->all = 2; break;
                case 'l':
                    op->list = 1; break;
                case 's':
                    op->size = 1; break;
                case 'p':
                    op->perm = 1; break;
                case 't':
                    op->time = 1; break;
                case 'R':
                    op->rec = 1; break;
                default:
                    break;
            }
        }
        if (argv[i][0] != '-' && argv[i][0] != 0)
        {
            ++(op->p->len); 
            op->p->paths = realloc(op->p->paths, sizeof(char*) * op->p->len);
            size_t alen = strlen(argv[i]);
            (op->p->paths)[op->p->len - 1] = malloc(sizeof(char) * (alen + 1));
            strncpy((op->p->paths)[op->p->len - 1], argv[i], alen + 1);
        }
    }
    if (op->perm || op->time || op->size)
        op->list = 1;
    return 0;
}


void clear_op(struct options *op)
{
    struct path *p = op->p;
    for (size_t i = 0; i < p->len; ++i)
        free(p->paths[i]);
    free(p->paths);
    free(p);
}
