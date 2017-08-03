# sfwatch
A simple filesystem watcher for the GNU Linux. This will work only on Linux systems. inotify and family doesn't work on
Mac OSX and other Unix systems. Inotify is a linux kernel subsystem API for watching filesystems.


Run sfwatch from any directroy like any other linux command
```bash
make all install 

```

```bash
This will monitor all activities in the directory and it files or monitor the specified file.
But will not go down to the subdirectories.
./sfwatch -p <dirpath> or <filepath>

```

```bash
To do recursive monitor on directory and it's subdirectories use -r option
./sfwatch -rp <dirpath>

```



##screenshot from how it really works


![alt text](https://github.com/seekaddo/sfwatch/blob/master/screentest.png)
