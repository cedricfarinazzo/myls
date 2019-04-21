#ifndef _ARG_H_
#define _ARG_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


int get_arg(struct options *op, int argc, char *argv[]);

void clear_op(struct options *op);

#endif /* _ARG_H_ */
