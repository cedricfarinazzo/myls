#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
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

struct entity {
    unsigned char type;
    char *name;
    struct stat *stat_file;
    size_t capacity;
    size_t nbchildreen;
    struct entity **child;
} entity;


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
        if (arg[0] != '-' && arg[0] != 0)
        {
            ++(op->p->len); 
            op->p->paths = realloc(op->p->paths, sizeof(char*) * op->p->len);
            size_t alen = strlen(arg);
            (op->p->paths)[op->p->len - 1] = malloc(sizeof(char) * (alen + 1));
            strncpy((op->p->paths)[op->p->len - 1], arg, alen + 1);
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
    free(p->paths);
    free(p);
}


struct entity *dirent_to_node(struct dirent *entry, char *entrypath)
{
    struct entity *node = malloc(sizeof(struct entity));
    node->type = entry->d_type;

    //concatenate path
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", entrypath, entry->d_name);

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
        errx(EXIT_FAILURE, "FILESYSTEM: stat failure");
    memcpy(node->stat_file, &fs, sizeof(struct stat));

    //initialyze childreen table
    node->nbchildreen = 0;
    node->capacity = 2;
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
        errx(EXIT_FAILURE, "stat failure");
    memcpy(tree->stat_file, &fs, sizeof(struct stat));

    //initialize childreen table
    tree->nbchildreen = 0;
    tree->capacity = 2;
    tree->child = malloc(tree->capacity * sizeof(struct entity*));

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
                continue;
            //get relative path
            snprintf(path, sizeof(path), "%s/%s", entrypath, entry->d_name);
            // add sub tree as a new childreen
            addnode(tree, build_tree(path));
        } else { // entry is not a directory
            // add file to tree as a new childreen
            addnode(tree,  dirent_to_node(entry, entrypath));
        }
    }
    closedir(dir);
    return tree;
}

void free_tree(struct entity *tree)
{
    for (size_t i = 0; i < tree->nbchildreen; ++i)
    {
        free_tree(tree->child[i]);
    }
    free(tree->child);
    free(tree->name);
    free(tree->stat_file);
    free(tree);
}

void print_tree(struct entity *tree, size_t indent)
{
    char indents[indent + 1];
    for (size_t i = 0; i < indent; ++i)
        indents[i] = ' ';
    indents[indent] = 0;
    printf("%s node(%p): %s | %d | nbchild(%ld / %ld) | mode(%d) | size %ld o\n", indents, 
           (void*)tree, tree->name, tree->type, tree->nbchildreen, tree->capacity, 
           tree->stat_file->st_mode, tree->stat_file->st_size);
    for (size_t i = 0; i < tree->nbchildreen; ++i)
    {
        print_tree(tree->child[i], indent + 4);
    }
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
