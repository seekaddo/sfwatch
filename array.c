//
// Created by Addo Dennis on 22/07/2017.
//
 
 
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "array.h"
 
 
//static size_t initsize = 5;


sds_array init_sds_array(size_t size){
    sds_array sda;
    
    
    sda.mem = malloc(size * sizeof(char *));
    if(sda.mem == NULL){
        fprintf(stderr,"malloc: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(sda.mem,0,size);
    sda.capacity = size;
    sda.size = 0;
    return sda;
}


void add_str(sds_array *sdsa,const char *s){
    if(sdsa->mem == NULL){
        fprintf(stderr,"Please init sdsa first\n");
        return;
    }
    static size_t l = 0;     // using l is not classic but just for easy readability as sda->mem[sda->size]
    
    if(l == sdsa->capacity){
        sdsa->capacity *= 2;
        char **mem= realloc(sdsa->mem,sdsa->capacity * sizeof(char *));
        if(mem == NULL){
            fprintf(stderr,"realloc: %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }

        sdsa->mem = mem;
    }
    
    size_t len = strlen(s)+1;
    
    sdsa->mem[l] = malloc(len * sizeof(char));
    if(sdsa->mem[l] == NULL){
        fprintf(stderr,"malloc: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    snprintf(sdsa->mem[l],len,"%s",s);
    sdsa->size = ++l;
    
    
}


inline int search_indx(size_t *ind, const sds_array *sda, const char *path){

    int found = -1;

    for (size_t i = 0; i < sda->size ; ++i) {
        if(strcmp(sda->mem[i],path) == 0){
            found = 0;
            *ind = i;
            break;
        }
    }


    return found;

}



void delete_str(sds_array *array, const size_t index){
    
    if(array->mem == NULL){
        fprintf(stderr,"delete_str: sds_array is empty.!\n");
        exit(EXIT_FAILURE);
    }


    if(index < array->size){
        free(array->mem[index]);
        array->mem[index] = NULL;
    } else{
        exit(EXIT_FAILURE);
    }
    
    //resizing the elements is Big O n O(n) when element is index = 0
    // is a pain but will keep things in order.

    for (size_t i = index; i < array->size ; ++i) {

        array->mem[i] =  (i != array->size-1)  ? array->mem[i+1]: NULL;

    }
    
    array->size -= 1;
    
}

void freeall(sds_array *sds,size_t len){
    for (size_t i = 0; sds->mem[i] != NULL && i <len ; ++i) {
        free(sds->mem[i]);
        sds->mem[i] = NULL;
    }
    free(sds->mem);
}

