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
#include <unistd.h>
#include <ctype.h>
#include "array.h"
 
static void do_files(const char *path, Array *str);
 
static void do_dir(const char *path, Array *fp);
 
static void handle_events(int fd, int *wd, size_t argc, Array *argv);
 
static void args_parser(int argc, char **argv);
 
static void do_usage(void);
 
static void flush(void);
 
 
/**********User defined constants here*/
static int rflag = 0;
static const char *pflag = "?";
static const char *sflag = "?";
 
 
/**
 *@brief Processing the passed commandline arguments
 *
 */
void args_parser(int argc, char **argv) {
     
    int opt;
     
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
                pflag = optarg;
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
 
 
#if 1
 
static void handle_events(int fd, int *wd, size_t argc, Array *argv) {
     
    char buf[4096]
    __attribute__ ((aligned (__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    size_t i;
    ssize_t len;
    char *ptr;
     
     
    for (;;) {
         
        /* Read some events.
         *  I will use ioctl() for the buffer size later. is more robust
         * than the work around with the gcc attribute aligned 4096 bytes
         */
         
         
         
        len = read(fd, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            freeall(argv, argc);
            (void) close(fd); //free other memories too
            flush();
            exit(EXIT_FAILURE);
        }
         
         
        if (len <= 0)
            break;
         
        /* Loop over all events in the buffer */
         
        for (ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {
             
            event = (const struct inotify_event *) ptr;
             
             
            /*Additional functions to process this information will be updated soon.*/
             
            if (event->mask & IN_OPEN)
                fprintf(stdout, "IN_OPEN: ");
            if (event->mask & IN_CLOSE_NOWRITE)
                fprintf(stdout, "IN_CLOSE_NOWRITE: ");
            if (event->mask & IN_CLOSE_WRITE)
                fprintf(stdout, "IN_CLOSE_WRITE: ");
            if (event->mask & IN_CREATE) {
                fprintf(stdout, "IN_CREATE: ");
            }
            if (event->mask & IN_DELETE) {
                fprintf(stdout, "IN_DELETE: ");
            }
             
            if (event->mask & IN_ACCESS) {
                fprintf(stdout, "IN_ACCESS: ");
            }
             
            if (event->mask & IN_DELETE_SELF) {
                fprintf(stdout, "IN_DELETE_SELF :");
            }
             
            if (event->mask & IN_MODIFY) {
                fprintf(stdout, "IN_MODIFY: ");
            }
             
            if (event->mask & IN_ISDIR) {
                fprintf(stdout, "IN_ISDIR: ");
            }
             
            if (event->mask & IN_OPEN) {
                fprintf(stdout, "IN_OPEN: ");
            }
             
            if (event->mask & IN_MOVED_FROM) {
                fprintf(stdout, "IN_MOVED_FROM: ");
            }
            if (event->mask & IN_MOVED_TO) {
                fprintf(stdout, "IN_MOVED_TO: ");
            }
             
             
            /* Print the name of the watched directory */
             
            for (i = 1; i < argc; ++i) {
                if (wd[i] == event->wd) {
                    fprintf(stdout, "%s/", (*argv)[i]);
                    break;
                }
            }
             
            /* Print the name of the file */
             
            if (event->len)
                fprintf(stdout, "%s", event->name);
             
            /* Print type of filesystem object */
             
            if (event->mask & IN_ISDIR)
                fprintf(stdout, " [directory]\n");
            else
                fprintf(stdout, " [file]\n");
        }
    }
}
 
#endif
 
static void do_files(const char *path, Array *fnames) {
     
    if (path == NULL) {
        fprintf(stderr, "%s\n", "Path can't be NULL, call help Usage");
        flush();
        exit(EXIT_FAILURE);
    }
     
     
    const char *newpath;
    static int iflag = 0;
     
     
    if (strcmp(path, " ") == 0) {
        newpath = "./";
         
    } else {
        newpath = path;
    }
     
     
    fprintf(stdout, "addingg:---> %s\n", newpath);
    add_str(fnames, newpath);
     
     
    struct stat fatr;
    int ret = lstat(newpath, &fatr);
     
     
    if (ret == -1) {
        fprintf(stderr, "lstat: %s %s\n", strerror(errno), newpath);
        flush();
        exit(EXIT_FAILURE);
    }
     
    if (S_ISDIR(fatr.st_mode) && rflag == 1) {
        do_dir(newpath, fnames);
    } else if (S_ISDIR(fatr.st_mode) && iflag == 0) {
        iflag = 1;
        do_dir(newpath, fnames);
    }
     
     
}
 
 
static void do_dir(const char *path, Array *fps) {
     
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
         
         
        if (drent->d_type == DT_DIR) do_files(p, fps);
        else
            do_files(p, fps);
         
         
    }
     
     
    (void) closedir(dir);
     
     
    return;
     
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
    size_t i;
     
     
    args_parser(argc, argv);
     
     
#if 0
    fprintf(stdout, "pflag: %s rflag: %d sflag: %s\n",pflag,rflag,sflag);
     
#endif
     
     
     
    /* Create the file descriptor  */
     
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "inotify_init1: %s\n", strerror(errno));
        flush();
        return EXIT_FAILURE;
    }
     
    /* Allocate memory for watch descriptors files/dir full paths */
     
    Array fnames = init_string(6);
     
    if (pflag[0] != '?') {
        do_files(pflag, &fnames);   //get all the files and dir and store them in fnames
    }
     
    size_t dlen = get_size(); //number of files and directories
     
    fprintf(stdout, "The get_size is: %zu\n", dlen);
     
    int wd[dlen];
     
    for (i = 0; fnames[i] != NULL && i < dlen; i++) {
        wd[i] = inotify_add_watch(fd, fnames[i], IN_ALL_EVENTS);
        if (wd[i] == -1) {
            fprintf(stderr, "Cannot watch '%s': %s \n", fnames[i], strerror(errno));
            freeall(&fnames, dlen);
            (void) close(fd);
            flush();
            exit(EXIT_FAILURE);
        }
         
        fprintf(stdout, "Watching: %s\n", fnames[i]);
    }
     
    fprintf(stdout, "\n");
     
    fprintf(stderr, "Press ENTER key to terminate.\n");
    /* Prepare for polling */
     
    nfds = 2;
     
    /* Console/terminal input */
     
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
     
    /* Inotify fd input */
     
    fds[1].fd = fd;
    fds[1].events = POLLIN;
     
    /* Wait for events and terminal input */
     
    fprintf(stdout, "Listening for events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
         
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
             
            fprintf(stderr, "poll: %s\n", strerror(errno));
            freeall(&fnames, dlen);
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
                    freeall(&fnames, dlen);
                    (void) close(fd);
                    flush();
                    exit(EXIT_FAILURE);
                }
                 
                break;
            }
             
            if (fds[1].revents & POLLIN) {
                 
                handle_events(fd, wd, dlen, &fnames);
            }
        }
    }
     
    fprintf(stdout, "Listening for events stopped.\n"
            "Cleaning up resources now!!\n");
     
     
    freeall(&fnames, dlen);
    (void) close(fd);
    flush();
    exit(EXIT_SUCCESS);
     
}