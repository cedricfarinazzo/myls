#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <err.h>
#include "arg.h"

#include "scanner.h"

struct entity *dirent_to_node(struct dirent *entry, char *entrypath)
{
    struct entity *node = malloc(sizeof(struct entity));
    node->type = entry->d_type;

    //concatenate path
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", entrypath, entry->d_name);
    if (access(path, R_OK) != 0)
    {
        free(node);
        warnx("cannot access to %s: permission denied", path);
        return NULL;
    }

    //write path to node
    size_t lenname = strlen(path);
    node->name = malloc(sizeof(char) * (lenname+1));
    memcpy(node->name, path, sizeof(char) * lenname);
    node->name[lenname] = 0;

    //write stat structure to node
    node->stat_file = malloc(sizeof(struct stat));
    struct stat fs;
    int e = stat(node->name, &fs);
    if (e == -1)
        err(e, "cannot access to %s", node->name);
    memcpy(node->stat_file, &fs, sizeof(struct stat));

    //initialyze childreen table
    node->nbchildreen = 0;
    if (!S_ISREG(fs.st_mode))
        node->capacity = 2;
    else
        node->capacity = 0;
    node->child = malloc(node->capacity * sizeof(struct entity*));
    return node;
}


void double_capacity(struct entity *tree)
{
    if (tree->capacity == tree->nbchildreen)
    {
        tree->capacity *= 2;
        tree->child = realloc(tree->child, sizeof(struct entity*) * tree->capacity);
    }
}


void addnode(struct entity *tree, struct entity *node)
{
    double_capacity(tree);
    tree->child[tree->nbchildreen] = node;
    tree->nbchildreen++;
}


struct entity *build_tree(char *entrypath)
{
    //create tree
    struct entity *tree = malloc(sizeof(struct entity));

    //write path to tree
    size_t lenname = strlen(entrypath);
    tree->name = malloc(sizeof(char) * (lenname+1));
    memcpy(tree->name, entrypath, sizeof(char) * lenname);
    tree->name[lenname] = 0;

    //write stat structure to tree
    tree->stat_file = malloc(sizeof(struct stat));
    struct stat fs;
    int e = stat(tree->name, &fs);
    if (e == -1)
        err(e, "cannot access to %s", tree->name);
    memcpy(tree->stat_file, &fs, sizeof(struct stat));

    //initialize childreen table
    tree->nbchildreen = 0;
    if (!S_ISREG(fs.st_mode))
        tree->capacity = 2;
    else
        tree->capacity = 0;
    tree->child = malloc(tree->capacity * sizeof(struct entity*));

    if (S_ISREG(fs.st_mode))
        return tree;

    //open direntory
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(entrypath)))
    {
        warnx("cannot access to %s: permission denied", entrypath);
        free(tree->name); free(tree->child); free(tree->stat_file); free(tree);
        return NULL;
    }

    //read directory content
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) { // entry is a directory
            char path[PATH_MAX];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                struct entity *new_node = dirent_to_node(entry, entrypath);
                if (new_node != NULL)
                    addnode(tree,  new_node);
                continue;
            }
            //get relative path
            snprintf(path, sizeof(path), "%s/%s", entrypath, entry->d_name);

            // add sub tree as a new childreen
            struct entity *subtree = build_tree(path);
            if (subtree != NULL)
                addnode(tree, subtree);
        } else { // entry is not a directory
            // add file to tree as a new childreen
            struct entity *new_node = dirent_to_node(entry, entrypath);
            if (new_node != NULL)
                addnode(tree,  new_node);
        }
    }
    closedir(dir);
    return tree;
}


void free_tree(struct entity *tree)
{
    if (tree == NULL)
        return;
    for (size_t i = 0; i < tree->nbchildreen; ++i)
    {
        free_tree(tree->child[i]);
    }
    free(tree->child);
    free(tree->name);
    free(tree->stat_file);
    free(tree);
}


void print_debug_tree(struct entity *tree, size_t indent)
{
    if (tree == NULL)
        return;
    char indents[indent + 1];
    for (size_t i = 0; i < indent; ++i)
        indents[i] = ' ';
    indents[indent] = 0;
    if (tree->capacity == 0)
    {
        printf("%s FILE: %s | %d | mode(%d) | size %ld o\n", indents, 
               tree->name, tree->type, tree->stat_file->st_mode, tree->stat_file->st_size);
    }
    else
    {
        printf("%s DIR: %s | %d | nbchild(%ld / %ld) | mode(%d) | size %ld o\n", indents, 
               tree->name, tree->type, tree->nbchildreen, tree->capacity, 
               tree->stat_file->st_mode, tree->stat_file->st_size);
        for (size_t i = 0; i < tree->nbchildreen; ++i)
        {
            print_debug_tree(tree->child[i], indent + 4);
        }
    }
}

char *getBasename(char *filename)
{
    char *base = filename;
    while (*filename != 0)
    {
        if (*filename == '/')
        { base = filename; ++base; }
        ++filename;
    }

    return base;
}

void print_node(struct entity *node, struct options *op, size_t indent)
{
    char *name = getBasename(node->name);
    if (name[0] != 0 && name[0] == '.')
    {
        if (op->all == 0)
            return;
        if (op->all == 2)
        {
            if (name[0] == '.')
                return;
        }
    }

    char *color = ""; char *reset = ""; 
    if (op->color)
    {
        mode_t m = node->stat_file->st_mode;
        if (S_ISREG(m)) //file
            color = RESET;
        else if (S_ISDIR(m)) //directoty
            color = GRN;
        else if (S_ISCHR(m)) // character special file (a device like a terminal). 
            color = RED;
        else if (S_ISBLK(m)) // block special file (a device like a disk)
            color = YEL;
        else if (S_ISFIFO(m)) // FIFO special file, or a pipe
            color = MAG;
        else if (S_ISLNK(m)) // symbolic link
            color = BLU;
        else if (S_ISSOCK(m)) // socket
            color = MAG;
        else
            color = RESET;
        reset = RESET;
    }
    
    indent = indent >= 4 ? indent - 4 : 0;
    if (!op->list)
        indent = 0;
    char indentc[indent + 1]; indentc[indent] = 0;
    for(size_t i = 0; i < indent; ++i) indentc[i] = '-';

    char *sizes;
    char sizess[8]; for(size_t i = 0; i < 8; ++i) sizess[i] = 0;
    sizes = sizess;
    if (op->size)
        asprintf(&sizes, " %do ", (int)node->stat_file->st_size);

    char *tim;
    char tims[1]; tims[0] = 0; tim = tims;
    if (op->time)
    {
        char s[1000];
        struct tm * p = localtime(&(node->stat_file)->st_mtime);
        strftime(s, 1000, "%A, %B %d %Y", p);
        asprintf(&tim, " %s ", s);
    }
    printf("%s%s%s%s%s%s ", indentc, sizes, tim, color, name, reset);
    if (op->list)
        printf(" \n");
    if (op->size)
        free(sizes);
    if (op->time)
        free(tim);
}


void __print_tree(struct entity *tree, struct options *op, size_t indent)
{
    if (strcmp(".", tree->name) != 0)
        print_node(tree, op, indent);
    for(size_t i = 0; i < tree->nbchildreen; ++i)
    {
        if (op->rec)
            __print_tree(tree->child[i], op, indent + 4);
        if (strcmp(".", tree->name) == 0 && !op->rec)
            __print_tree(tree->child[i], op, 0);
    }
}

void print_tree(struct entity *tree, struct options *op)
{
    __print_tree(tree, op, 0);
}

