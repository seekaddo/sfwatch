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
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include "array.h"

/* User defined color for output formatting*/ 

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGNENTAP "\x1b[35m"
#define CYAN "\x1b[95m"
#define DARKGREAY "\x1b[90m"
#define RESET "\x1b[0m"




static int do_files(const char *path, sds_array *str);
 
static void do_dir(const char *path, sds_array *fp);
 
static void events_handler(int fd, int *wd, size_t argc, sds_array *argv);
 
static void args_parser(int argc, char **argv,char *pth[]);

 static int add_to_watchlist(int fd,int wdd[], size_t len, sds_array *ffname);

static int get_lfiles(char *sp[],int l,sds_array *f);
 
static void do_usage(void);
 
static void flush(void);
 
 
/**********User defined constants here*/
static int rflag = 0;
static const char *sflag = " ";
 
 
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
                //pflag = optarg;
                lpaths[indx++] = optarg;
                break;
            case 's':
                sflag = optarg;
                fprintf(stderr, "Sorry this option  is still under implementation. \n");
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

 
static void events_handler(int fd, int *wd, size_t argc, sds_array *argv) {
     
    
       char buf[4096]
    __attribute__ ((aligned (__alignof__(struct inotify_event))));
    const struct inotify_event *event;          //readonly inotify event
    size_t i;
    ssize_t len;
    char *ptr;
    //size_t buflen;
     
     
    for (;;) {
         
        /* Read some events.
         *  I will use ioctl() for the buffer size later. is more robust
         * than the work around with the gcc attribute aligned 4096 bytes
         */

#if 0
       //Portability problem with most Debian system. 4096 with gcc aligned is big but is ok.
       
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
            freeall(argv, argc);
            (void) close(fd);
            flush();
            exit(EXIT_FAILURE);
        }
         
         
        if (len <= 0)
            break;
         
        /* Loop over all events in the buffer */
         
        for (ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {
             
            event = (const struct inotify_event *) ptr;
             
             
            /*Additional functions to process this information will be updated soon.

            switch(event->mask & ~(IN_ALL_EVENTS | IN_UNMOUNT | IN_IGNORED | IN_Q_OVERFLOW))
            */
             
            if (event->mask & IN_OPEN)              fprintf(stdout, "IN_OPEN: ");
            if (event->mask & IN_ATTRIB)            fprintf(stdout, "IN_ATTRIB: ");

             if (event->mask & IN_CLOSE_NOWRITE)    fprintf(stdout, "IN_CLOSE_NOWRITE: ");

             if (event->mask & IN_CLOSE_WRITE)      fprintf(stdout, "IN_CLOSE_WRITE: ");

             if (event->mask & IN_CREATE)           fprintf(stdout, "IN_CREATE: ");
            
             if (event->mask & IN_DELETE)           fprintf(stdout, "IN_DELETE: ");
             
             if (event->mask & IN_ACCESS)           fprintf(stdout, "IN_ACCESS: ");
            
             if (event->mask & IN_DELETE_SELF)      fprintf(stdout, "IN_DELETE_SELF:");

             if (event->mask & IN_IGNORED)          fprintf(stdout, "IN_IGNORED:");
            
             if (event->mask & IN_MODIFY)           fprintf(stdout, "IN_MODIFY: ");
            
             if (event->mask & IN_ISDIR)            fprintf(stdout, "IN_ISDIR: ");
             
             if (event->mask & IN_MOVE_SELF)        fprintf(stdout, "IN_MOVED_SELF: ");
            
            if (event->mask & IN_MOVED_FROM)        fprintf(stdout, "IN_MOVED_FROM: ");
            
            if (event->mask & IN_MOVED_TO)          fprintf(stdout, "IN_MOVED_TO: ");

            //if (event->mask & IN_Q_OVERFLOW)        fprintf(stdout, "%sIN_Q_OVERFLOW: %s",RED,RESET);
             
            /* Print the name of the watched directory */
             
             const char *pth = " ";
             const char *name = "";
             const char *slash = "/";

            for (i = 0; i < argc; ++i) {
               
                if (wd[i] == event->wd) {
                    pth = argv->mem[i];
                    break;
                }
            }
             
            /* Print the name of the file */
             

            if (pth != NULL && event->len){
                name = event->name;

                size_t l = strlen(pth);
                slash =  (pth[l-1] == '/' ? "" :"/");

            fprintf(stdout, "%s%s%s%s %s%s \n",
                                GREEN, pth, slash,name,
                                (event->mask & IN_ISDIR)? "[directory]": "[file]",RESET);
                
            }else{
                fprintf(stdout, "%s\n", pth);
            }
            
            
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
     
     
    if (strcmp(path, " ") == 0) {
        newpath = "./";
         
    } else {
        newpath = path;
    }
     
     
    //fprintf(stdout, "addingg:---> %s\n", newpath);
    add_str(fnames, newpath);
     
     
    struct stat fatr;
    int ret = lstat(newpath, &fatr);
     
     
    if (ret == -1) {
        fprintf(stderr, "lstat: %s %s\n", strerror(errno), newpath);
        freeall(fnames,fnames->size);
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
 
 
static void do_dir(const char *path, sds_array *fps) {
     
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
     
    return;
}


int add_to_watchlist(int fdd,int wdd[], size_t len, sds_array *ffname){

    size_t i;
    int status = 0;

    for (i = 0; ffname->mem[i] != NULL && i < len; i++) {
        wdd[i] = inotify_add_watch(fdd, ffname->mem[i], IN_ALL_EVENTS | IN_DONT_FOLLOW);
        if (wdd[i] == -1) {
            fprintf(stderr, "Cannot watch '%s': %s \n", ffname->mem[i], strerror(errno));
            status =  -1;
            break;
        }
         
        fprintf(stdout, "Watching: %s\n", ffname->mem[i]);
    }
     
    fprintf(stdout, "\n");

    return status;

}

int get_lfiles(char *sp[],int l, sds_array *fn){

    for (int i = 0; sp[i] != NULL && i < l; ++i){

        if( do_files(sp[i], fn) == -1){   //get all the files and dir and store them in fnames
            return -1;
        }
    }

    return 0;       

}
 
void flush(void) {
    if (fflush(stdout) == EOF) {
        fprintf(stderr, "fflush: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
 
 
static void do_usage(void) {
    fprintf(stdout, "%s\n%s\n%s\n%s\n%s\n%s\n",
            "Usage: sfwatch <options> [Path to dir/file]",
            "Options: (You can use any of the following options)",
            "       : -h            Shows alll necessary information for sfwatch",
            "       : -p            path to the file/directory to monitor",
            "       : -r            recursive monitor for directory/subdirectories [no file]",
            "       : -s            output log to a file"
             
            );
          
}
 


 
int main(int argc, char *argv[]) {
    char buf;
    int fd, poll_num;
    nfds_t nfds;
    struct pollfd fds[2];

    char *lpath[argc];
    memset(lpath,0,argc);
     
     
    args_parser(argc, argv,lpath);

     
     
    /* Create the file descriptor  */
     
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "inotify_init1: %s\n", strerror(errno));
        flush();
        return EXIT_FAILURE;
    }
     
    /* Allocate memory for watch descriptors files/dir full paths */
     
    sds_array fnames = init_sds_array(6);



    if(get_lfiles(lpath,argc,&fnames) == -1){//get all the files and dir and store them in fnames
     
        freeall(&fnames,fnames.size );
        (void) close(fd);

        fprintf(stderr, "Sorry Listening for events stopped.!!\n"
                        "Cleaning up resources now!!!\n");
        flush();
        return EXIT_FAILURE;
    }  
    
    //size_t dlen = get_size(); //number of files and directories
   


    //fprintf(stdout, "The get_size is: %zu\n", fnames.size);
     
    int wd[fnames.size];


    if (add_to_watchlist(fd,wd,fnames.size, &fnames) == -1){

        freeall(&fnames, fnames.size);
        (void) close(fd);
        fprintf(stdout, "Listening for events stopped.\n"
                    "Cleaning up resources now!!\n");
        flush();
        return EXIT_FAILURE;
    }     
    fprintf(stderr, "Press ENTER key to terminate.\n");
    


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
                    flush();
                    exit(EXIT_FAILURE);
                }
                 
                break;
            }
             
            if (fds[1].revents & POLLIN) {
                 
                events_handler(fd, wd, fnames.size, &fnames);
            }
        }
    }
     
    fprintf(stdout, "Listening for events stopped.\n"
                    "Cleaning up resources now!!\n");
     
     
    freeall(&fnames, fnames.size);
    (void) close(fd);
    flush();
    exit(EXIT_SUCCESS);
     
}
