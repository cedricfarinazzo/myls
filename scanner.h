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
void print_tree(struct entity *tree, size_t indent);


#endif /* _SCANNER_H_ */
