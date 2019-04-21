#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <err.h>

#include "scanner.h"


int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

struct entity *dirent_to_node(struct dirent *entry, char *entrypath)
{
    struct entity *node = malloc(sizeof(struct entity));
    node->type = entry->d_type;

    //concatenate path
    char path[1024];
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
        free(tree->child); free(tree->stat_file); free(tree);
        return NULL;
    }

    //read directory content
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) { // entry is a directory
            char path[1024];
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
            addnode(tree, build_tree(path));
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
    printf("%s node(%p): %s | %d | nbchild(%ld / %ld) | mode(%d) | size %ld o\n", indents, 
           (void*)tree, tree->name, tree->type, tree->nbchildreen, tree->capacity, 
           tree->stat_file->st_mode, tree->stat_file->st_size);
    for (size_t i = 0; i < tree->nbchildreen; ++i)
    {
        print_debug_tree(tree->child[i], indent + 4);
    }
}
