//
// Created by Addo Dennis on 22/07/2017.
//


#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "string.h"


static size_t initsize = 5;
static size_t current_size = 0;

string init_string(size_t size) {
    
    if (size != initsize) {
        initsize = size;
    }
    string str = malloc(size * sizeof(char *));
    if (str == NULL) {
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(str, 0, size);
    return str;
}

void add_str(string *str, const char *s) {
    if (*str == NULL) {
        printf("Please init str first\n");
        return;
    }
    static size_t l = 0;
    
    if (l == initsize) {
        initsize *= 2;
        *str = realloc(*str, initsize * sizeof(char *));
        if (*str == NULL) {
            fprintf(stderr, "realloc: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    
    size_t len = strlen(s) + 1;
    
    (*str)[l] = malloc(len * sizeof(char));
    snprintf((*str)[l], len, "%s", s);
    current_size = l;
    l++;
    
    
}

size_t get_size(void) {
    return (current_size != 0 && current_size != initsize) ? current_size + 1 : initsize;
}


void freeall(string *ss, size_t len) {
    for (size_t i = 0; (*ss)[i] != NULL && i < len; ++i) {
        free((*ss)[i]);
        (*ss)[i] = NULL;
    }
    free(*ss);
}



