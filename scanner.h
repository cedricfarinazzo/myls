#ifndef _SCANNER_H__
#define _SCANNER_H__

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <err.h>
#include <time.h>
#include "arg.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

struct entity {
    unsigned char type;
    char *name;
    struct stat *stat_file;
    size_t capacity;
    size_t nbchildreen;
    struct entity **child;
} entity;


struct entity *dirent_to_node(struct dirent *entry, char *entrypath);


void double_capacity(struct entity *tree);


void addnode(struct entity *tree, struct entity *node);


struct entity *build_tree(char *);


void free_tree(struct entity *tree);


void print_debug_tree(struct entity *tree, size_t indent);


void print_tree(struct entity *tree, struct options *op);


#endif /* _SCANNER_H_ */
