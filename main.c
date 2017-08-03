/*
MIT License

Copyright (c) 2017 Dennis Addo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */






/**
 *@file main.c
 *   A simple file watcher for the linux system.
 *   @author Dennis Addo
 *   @date 22/07/2017
 **/

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <fnmatch.h>
#include <libgen.h>
#include "array.h"

/* User defined color for output formatting*/

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGNENTAP "\x1b[35m"
#define CYAN "\x1b[95m"
#define LCYAN "\x1b[96m"
#define DARKGREAY "\x1b[90m"
#define LGRAY "\x1b[100m"
#define LBLUE "\x1b[94m"
#define RESET "\x1b[0m"


static int do_files(const char *path, sds_array *str);

static void do_dir(const char *path, sds_array *fp);

static void events_handler(int fd, int **wd, sds_array *argv);

static void args_parser(int argc, char **argv, char *pth[]);

static int add_to_watchlist(int fd, int **wdd, size_t len, sds_array *ffname);

static int get_lfiles(char *sp[], int l, sds_array *f);

static int fexisit(const char *pathname);



static void do_delete_s(const int tp, char *pathname,sds_array *sda, size_t idx,int **wd);
static void do_delete(const int tp, char *pathname,sds_array *sda,  int **wd);

static void do_formatted_output(const int type, const char *pathname, const char *msg,const char **colrs);

static void do_move_from(const int typ, const char *pathname,sds_array *sda, int **wd);

static void do_move_to(const char *pathname);

static void get_time(char *buf, size_t s);

static void do_resize(int **wd,size_t indx, size_t size);

static void do_usage(void);

static void flush(void);


/**********User defined constants here*/
static int rflag = 0;
static int gflag = 0;
static const char *sflag = "?";

const char const *msg_open = "--> was open!";
const char const *msg_acess = "--> was accessed!";
const char const *msg_close = "--> was closed!";
const char const *msg_created = "--> was created!";
const char const *msg_delete = "--> was deleted!";
const char const *msg_modify = "--> The content was modified";


/**
 *@brief Processing the passed commandline arguments
 *
 */
void args_parser(int argc, char **argv, char *lpaths[]) {

    int opt;
    int indx = 0;

    opterr = 0;


    if (argc < 2) {
        do_usage();
        flush();
        exit(EXIT_FAILURE);
    }


    while ((opt = getopt(argc, argv, "hrp:s:")) != -1) {

        switch (opt) {
            case 'h':
                do_usage();
                break;
            case 'r':
                rflag = 1;
                break;
            case 'p':
                lpaths[indx++] = optarg;
                break;
            case 's':
                sflag = optarg;
                break;
            case '?':
                if (optopt == 'p') {
                    fprintf(stderr, "Option -%c requirs argument.\n", optopt);
                } else if (optopt == 's') {
                    fprintf(stderr, "Option -%c requirs argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unkown option `-%c `.\n", optopt);
                } else {
                    fprintf(stderr, "Uknown option character `\\x%x.`\n", optopt);
                }
                flush();
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "%s\n", strerror(errno));
                flush();
                exit(EXIT_FAILURE);

        }
    }

    if (optind < argc) {
        fprintf(stderr, "%s\n", "Non-Option argument");
        do_usage();
        flush();
        exit(EXIT_FAILURE);
    }


}


 void events_handler(int fd, int **wd,sds_array *argv) {


    char buf[4096]
            __attribute__ ((aligned (__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;
    char *ptr;


    for (;;) {

        /* Read some events.
         *  I will use ioctl() for the buffer size later. is more robust
         * than the work around with the gcc attribute aligned 4096 bytes
         */

#if 0
        
        //failing on most Linux system/ buffer aligned with gcc attribute is better
        if(ioctl(fd,FIONREAD,&buflen) == -1){
            fprintf(stderr, "ioctl: %s\n", strerror(errno));
            freeall(argv,argv->size);
            exit(EXIT_FAILURE);
        }
        
        char buf[buflen];


            printf("bytes available: %zu\n", buflen);
        if (buflen <= 0)
            break;
#endif


        len = read(fd, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            freeall(argv, argv->size);
            (void) close(fd);
            free(wd);
            flush();
            exit(EXIT_FAILURE);
        }


        if (len <= 0)
            break;

        /* Loop over all events in the buffer */

        for (ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *) ptr;


            const char *pth = "?";
            const char *name = "";
            const char *slash = "";

            size_t i,indx;

            for (i = 0; i < argv->size ; ++i) {

                if ((*wd)[i] == event->wd) {
                    pth = argv->mem[i];
                    break;
                }
            }

            indx = i;

            if (event->len) {
                name = event->name;

                size_t l = strlen(pth);
                slash = (pth[l - 1] == '/' ? "" : "/");

                i = l + strlen(name) + strlen(slash) + 1;

            } else {
                i = strlen(pth) + 1;

            }

            if( i <= 2)
                continue;


            char pthname[i];
            int type = (event->mask & IN_ISDIR) ? 1: 0;

            snprintf(pthname, i, "%s%s%s", pth, slash, name);


            /*Additional functions to process this information will be updated soon.


            */

            switch( event->mask & (IN_ALL_EVENTS | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED) ){

                case IN_OPEN :
                    do_formatted_output(type, pthname,msg_open,(const char *[]){GREEN,RESET});
                    break;

                case IN_ACCESS: case IN_ATTRIB:
                    do_formatted_output(type,pthname,msg_acess,(const char *[]){DARKGREAY,RESET});
                    break;

                case IN_CLOSE_WRITE: case IN_CLOSE_NOWRITE:

                    do_formatted_output(type,pthname,msg_close,NULL);
                    break;

                case IN_CREATE:
                    do_formatted_output(type,pthname,msg_created,(const char *[]){GREEN,RESET});
                    break;

                case IN_DELETE:
                    do_delete(type,pthname,argv,wd);
                    break;
                case IN_DELETE_SELF:
                    do_delete_s(type,pthname,argv,indx,wd);
                    break;

                case IN_MODIFY:
                    do_formatted_output(type,pthname,msg_modify,(const char *[]){CYAN,RESET});
                    break;

                case IN_MOVED_FROM:
                case IN_MOVE_SELF:
                    do_move_from(type,pthname, argv,wd);
                    break;

                case IN_MOVED_TO:
                    do_move_to(pthname);
                    break;

                default:
                    break;
            }


        }
    }
}


/**\brief
 * formatting the output from the inotify events
 * to a speficied file or to STDOUT
 *
 * Since plain text files doesn't support ascii color
 * output coloring is remove here.
 *
 */

inline void do_formatted_output(const int t, const char *pathname, const char *msg,const char **colrs){


    char tbuf[30];
    struct tm ftm;
    time_t time1;



    time1 = time(NULL);
    localtime_r(&time1,&ftm);

    strftime(tbuf,sizeof tbuf,"%F %T",&ftm);
    const char *type = (t == 1) ? "[Directory]": "[File]";

    FILE *fp = NULL;

    if(sflag[0] != '?' && (fp = fopen(sflag,"a")) == NULL){
        fprintf(stderr,"fopen:%s \n",strerror(errno));
        exit(1);

    }



    if(colrs == NULL){
        fprintf((fp == NULL? stdout: fp),"%s %s %s %s\n",tbuf,type,pathname,msg);
    }else {
        fprintf((fp == NULL? stdout: fp),"%s %s %s %s %s %s\n",tbuf,type,
                                        fp == NULL ? colrs[0]:"",pathname,msg,fp == NULL ? colrs[1]:"");
    }


    if(fp != NULL){
        if(fclose(fp) == EOF){
            fprintf(stderr,"fopen:%s \n",strerror(errno));
            exit(1);
        }
    }

}


inline void do_move_from(const int t, const char *pathname, sds_array *sda,int **wd){

     if(pathname == NULL){
         return;
     }

     const char *mg = "moved_from: ";
     const char *type = (t == 1) ? "[Directory]": "[File]";

     char tbuf[30];

     get_time(tbuf,30);


    size_t gin;
    int sear = search_indx(&gin,sda,pathname);


     if(sear == 0 && fexisit(pathname) == -1){

         (*wd)[gin] = -1;
         do_resize(wd,gin,sda->size);
         delete_str(sda,gin);
     }


    FILE *fp = NULL;

    if(sflag[0] != '?' && (fp = fopen(sflag,"a")) == NULL){
        fprintf(stderr,"fopen:%s \n",strerror(errno));
        exit(1);

    }



    fprintf((fp == NULL? stdout: fp),"%s %s %s %s%s%s ->",tbuf,type,mg,
                                    fp == NULL ? LGRAY:"",pathname,fp == NULL ? RESET:"");



    if(fp != NULL){
        if(fclose(fp) == EOF){
            fprintf(stderr,"fopen:%s \n",strerror(errno));
            exit(1);
        }
    }

 }

 inline void do_move_to(const char *pathname){
     FILE *fp = NULL;

     if(sflag[0] != '?' && (fp = fopen(sflag,"a")) == NULL){
         fprintf(stderr,"fopen:%s \n",strerror(errno));
         exit(1);

     }



     if(pathname == NULL || strlen(pathname) <= 2){
         fprintf((fp == NULL? stdout: fp),"\n");
     } else{
         fprintf((fp == NULL? stdout: fp)," %s \n",pathname);
     }


     if(fp != NULL){
         if(fclose(fp) == EOF){
             fprintf(stderr,"fopen:%s \n",strerror(errno));
             exit(1);
         }
     }

 }

/**\brief
 *
 * simple current time formatter
 * */
 inline void get_time(char *tbuf, size_t s){
    struct tm ftm;
    time_t time1;

    time1 = time(NULL);
    localtime_r(&time1,&ftm);

    strftime(tbuf,s,"%F %T",&ftm);

}


/**\brief
 *
 * handles the formatting output for delete and delete_self
 *
 * */


void do_delete_s(const int tp, char *pathname,sds_array *sda, size_t idx, int **wd) {

    const char *msg = "--> was deleted!";
    const char *type = (tp == 1) ? "[Directory]": "[File]";
    int l = fexisit(pathname);

    char tbuf[30];
    get_time(tbuf,30);


    FILE *fp = NULL;

    if(sflag[0] != '?' && (fp = fopen(sflag,"a") ) == NULL){
        fprintf(stderr,"fopen:%s \n",strerror(errno));
        exit(1);

    }




    if( l == 0){
        fprintf(stdout,"\n");
    } else if (l == -1 && sda->mem[idx] != NULL){

        if (strcmp(sda->mem[idx],pathname) == 0) { // is it an object we are monitoring
            (*wd)[idx] = -1;
            do_resize(wd,idx,sda->size);
            delete_str(sda,idx);
            fprintf((fp == NULL? stdout: fp),"%s %s %s %s %s %s\n",tbuf ,type,
                                             fp == NULL ? RED:"",pathname,msg,fp == NULL ? RESET:" ");
            gflag =1;

        }else{
            fprintf((fp == NULL? stdout: fp),"%s %s %s %s %s %s\n",tbuf ,type,
                                           fp == NULL ? RED: "",pathname,msg,fp == NULL ? RESET: "");
        }
    }


    if(fp != NULL){
        if(fclose(fp) == EOF){
            fprintf(stderr,"fopen:%s \n",strerror(errno));
            exit(1);
        }
    }


}





void do_delete(const int tp, char *pathname,sds_array *sda,  int **wd){
    if(gflag == 1)
        return;

    size_t gin;
    int sear = search_indx(&gin,sda,pathname);


    int status = fexisit(pathname);
    const char *type = (tp == 1) ? "[Directory]": "[File]";
    char tbuf[30];

    get_time(tbuf,30);


    if(status == 0){
        return;
    }


    FILE *fp = NULL;

    if(sflag[0] != '?' && (fp = fopen(sflag,"a")) == NULL){
        fprintf(stderr,"fopen:%s \n",strerror(errno));
        exit(1);

    }



    if (status == -1) {

        if (sear != -1) {
            (*wd)[gin] = -1;
            do_resize(wd, gin, sda->size);
            delete_str(sda, gin);
            fprintf((fp == NULL? stdout: fp), "%s %s %s %s %s %s\n", tbuf, type,
                                               fp == NULL ? RED:"", pathname, msg_delete,fp == NULL ?RESET:"");

        } else {
            fprintf((fp == NULL? stdout: fp), "%s %s %s %s %s %s\n", tbuf, type,
                                             fp == NULL ? RED:"", pathname, msg_delete,fp == NULL? RESET:"");
        }
    }

    if(fp != NULL){
        if(fclose(fp) == EOF){
            fprintf(stderr,"fopen:%s \n",strerror(errno));
            exit(1);
        }
    }


}





int do_files(const char *path, sds_array *fnames) {

    if (path == NULL) {
        fprintf(stderr, "%s\n", "Path can't be NULL, call help Usage");
        return -1;
    }


    const char *newpath;
    static int iflag = 0;
    const char *curr_path = "./";


    if (strcmp(path, " ") == 0) {
        newpath = curr_path;

    } else {
        newpath = path;
    }

#if 0
    fprintf(stdout, "addingg:---> %s\n", newpath);
#endif

    add_str(fnames, newpath);


    struct stat fatr;
    int ret = lstat(newpath, &fatr);


    if (ret == -1) {
        fprintf(stderr, "lstat: %s %s\n", strerror(errno), newpath);
        freeall(fnames, fnames->size);
        flush();
        exit(EXIT_FAILURE);
    }

    if (S_ISDIR(fatr.st_mode) && rflag == 1) {
        do_dir(newpath, fnames);
    } else if (S_ISDIR(fatr.st_mode) && iflag == 0) {
        iflag = 1;
        do_dir(newpath, fnames);
    }

    return 0;


}


 void do_dir(const char *path, sds_array *fps) {

    struct dirent *drent;

    DIR *dir = opendir(path);

    if (dir == NULL) {
        fprintf(stderr, "opendir: %s\n", strerror(errno));
        return;
    }


    while ((drent = readdir(dir)) != NULL) {

        if (strcmp(drent->d_name, ".") == 0 || strcmp(drent->d_name, "..") == 0) {
            continue;
        }


        const char *slash = "";
        size_t len = strlen(path);

        if (path[len - 1] != '/') slash = "/";
        len += strlen(drent->d_name) + 2;

        char p[len];
        snprintf(p, len, "%s%s%s", path, slash, drent->d_name);

        do_files(p, fps);


    }

    (void) closedir(dir);

}


int add_to_watchlist(int fdd, int **wdd, size_t len, sds_array *ffname) {

    size_t i;
    int status = 0;

    for (i = 0; ffname->mem[i] != NULL && i < len; i++) {
        (*wdd)[i] = inotify_add_watch(fdd, ffname->mem[i], IN_ALL_EVENTS | IN_DONT_FOLLOW);
        if ((*wdd)[i] == -1) {
            fprintf(stderr, "Cannot watch '%s': %s \n", ffname->mem[i], strerror(errno));
            status = -1;
            break;
        }
 //       printf("Watchinf index[%d] --> %s\n",(*wdd)[i],ffname->mem[i]);

    }
  //  printf("\n");

    return status;

}

int get_lfiles(char *sp[], int l, sds_array *fn) {

    for (int i = 0; sp[i] != NULL && i < l; ++i) {

        if (do_files(sp[i], fn) == -1) {   //get all the files and dir and store them in fnames
            return -1;
        }
    }

    return 0;

}


 inline void do_resize(int **wd,size_t indx, size_t size){
     if(indx >= size)
         return;

     for (size_t i = indx; i < size ; ++i) {
         (*wd)[i] = i != size -1 ? (*wd)[i+1]:0;
     }

     *wd = realloc(*wd,(size -1) * sizeof(int));

 }




/**\brief check if a file a exisit or not
 *        This is needed to determine or confirm
 *        some unexpected actions from vim modify
 *        that triggers IN_DELETE_self
 * @retval 0 on success and -1 on failure with errno
 *
 * */
inline int fexisit(const char *pathname) {
    FILE *r;

    r = fopen(pathname,"r");
    if (r == NULL && errno != EINVAL) {

        return  -1;
    }

    if(r == NULL && errno != 0){
        fprintf(stderr,"fexisit: %s\n",strerror(errno));
        exit(1);
    }

    if(fclose(r) == EOF){
        fprintf(stderr,"fopen:%s \n",strerror(errno));
        exit(1);
    }

    return 0;
}


void flush(void) {
    if (fflush(stdout) == EOF) {
        fprintf(stderr, "fflush: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}


inline  void do_usage(void) {
    fprintf(stdout, "%s\n%s\n%s\n%s\n%s\n%s\n",
            "Usage: sfwatch <options> [Path to dir/file]",
            "Options: (You can use any of the following options)",
            "       : -h            Shows alll necessary information for sfwatch",
            "       : -p            path to the file/directory to monitor",
            "       : -r            recursive monitor for directory/subdirectories [no file]",
            "       : -s            output log to a file"

    );

}



/**\brief
 * Main program start here. Initialise all the necessary buffers
 * and use poll to listen on file discriptors for buffer feeds.
 *
 */

int main(int argc, char *argv[]) {
    char buf;
    int fd, poll_num;
    nfds_t nfds;
    struct pollfd fds[2];

    char *lpath[argc];
    memset(lpath, 0, argc);


    args_parser(argc, argv, lpath);



    /* Create the file descriptor  */

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "inotify_init1: %s\n", strerror(errno));
        flush();
        return EXIT_FAILURE;
    }

    /* Allocate memory for watch descriptors files/dir full paths */

    sds_array fnames = init_sds_array(6);


    if (get_lfiles(lpath, argc, &fnames) == -1) {//get all the files and dir and store them in fnames

        freeall(&fnames, fnames.size);
        (void) close(fd);

        fprintf(stderr, "Sorry Listening for events stopped.!!\n"
                "Cleaning up resources now!!!\n");
        flush();
        return EXIT_FAILURE;
    }




    fprintf(stdout, "%sCurrently monitoring %zu objects%s\n",YELLOW, fnames.size,RESET);

    int *wd = malloc(fnames.size * sizeof(int));

    if(wd == NULL){
        fprintf(stderr,"malloc: %s\n",strerror(errno));
        freeall(&fnames,fnames.size);
        (void)close(fd);
        flush();
        return EXIT_FAILURE;
    }


    if (add_to_watchlist(fd, &wd, fnames.size, &fnames) == -1) {

        freeall(&fnames, fnames.size);
        (void) close(fd);
        fprintf(stdout, "Listening for events stopped.\n"
                "Cleaning up resources now!!\n");
        free(wd);
        flush();
        return EXIT_FAILURE;
    }



    fprintf(stdout, "%sRedirrecting all log to: %s %s\n",YELLOW,(sflag[0] != '?'? sflag:"STDOUT"),RESET);
    fprintf(stdout, "%sPress ENTER key to terminate.%s\n",RED,RESET);


    /* Prepare for polling 2 file discriptors for now*/

    nfds = 2;

    /* Console/terminal input */

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Inotify fd input */

    fds[1].fd = fd;
    fds[1].events = POLLIN;

    /* Wait for events --> fd and terminal input */

    fprintf(stdout, "Listening for events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);

        if (poll_num == -1) {
            if (errno == EINTR)
                continue;

            fprintf(stderr, "poll: %s\n", strerror(errno));
            freeall(&fnames, fnames.size);
            (void) close(fd);
            free(wd);
            flush();
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0) {

            if (fds[0].revents & POLLIN) {

                /* Console input is available. Empty stdin until newline and quit */
                ssize_t sbf;


                while ((sbf = read(STDIN_FILENO, &buf, 1)) > 0 && buf != '\n')
                    continue;

                if (sbf == -1) {
                    fprintf(stderr, "read: %s\n", strerror(errno));
                    freeall(&fnames, fnames.size);
                    (void) close(fd);
                    free(wd);
                    flush();
                    exit(EXIT_FAILURE);
                }

                break;
            }

            if (fds[1].revents & POLLIN) { // data from inotify event is ready. handle them.

                events_handler(fd, &wd, &fnames);
            }
        }
    }

    fprintf(stdout, "Listening for events stopped.\n"
            "Cleaning up resources now!!\n");


    freeall(&fnames, fnames.size);
    (void) close(fd);
    free(wd);
    flush();
    exit(EXIT_SUCCESS);

}
