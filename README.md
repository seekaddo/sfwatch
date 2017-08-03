# sfwatch
A simple filesystem watcher for the GNU Linux. This will work only on Linux systems. inotify and family doesn't work on
Mac OSX and other Unix systems. Inotify is a linux kernel subsystem API for watching filesystems.


Run sfwatch from any directroy like any other linux command
```bash
make all install 

```


This will monitor all activities in the directory and it's files or monitor the specified file,
but will not go down to the subdirectories.
```bash
./sfwatch -p <dirpath> or <filepath>

```

To do recursive monitor on directory and it's subdirectories use -r option
```bash
./sfwatch -rp <dirpath>

```


You can redirect output to a file with the -s option

```bash
./sfwatch -rp <dirpath> -s path_tofile
```



# screenshot to show how it really works


![alt text](https://github.com/seekaddo/sfwatch/blob/master/screentest.png)

![alt text](https://github.com/seekaddo/sfwatch/blob/master/screentest2.png)

![alt text](https://github.com/seekaddo/sfwatch/blob/master/screentest3.png)



# feedbacks

I am open to all kinds of feedbacks. Little project to keep me engage this sommer2017.
Give it a try and let me know what you think. Feel free to open an issue for your feedbacks
