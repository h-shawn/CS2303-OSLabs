# Prj3README

#### Project Structure

```shell
proj3
├── OS_proj3.pdf
├── Prj3README.md
├── step1
│   ├── disk.c
│   ├── header.h
│   ├── Makefile
│   └── typescript.txt
├── step2
│   ├── fs.c
│   ├── header.h
│   ├── Makefile
│   └── typescript.txt
├── step3
│   ├── client.c
│   ├── disk.c
│   ├── fs.c
│   ├── header.h
│   ├── Makefile
│   └── typescript.txt
└── typescript.txt
```



#### Design a basic disk-storage system

- Source file name: disk.c
- Executable file name: disk
- Command line: ./disk [cylinders] [sector per cylinder] [track-to-track delay] [disk-storage-filename]

Usage:

```shell
./disk <cylinders> <sector per cylinder> <track-to-track delay> <disk-storage-filename>
```

Protocol:

- **I**: Information request. The disk returns two integers representing the disk geometry: the number of cylinders and the number of sectors per cylinder.

- **R c s**: Read request for the contents of cylinder c sector s. The disk returns **Yes** followed by a writespace and those 256 bytes of information, or **No** if no such block exists.
- **W c s data**: Write a request for cylinder c sector s. The disk returns **Yes** and writes the data to cylinder c sector s if it is a valid write request or returns **No** otherwise.
- **E**: Exit the disk-storage system.



#### Design a basic file system

- Source file name: fs.c
- Executable file name: fs
- Command line: ./fs

Usage:

```shell
./fs
```

Protocol:

- **f** : Format. This will format the file system on the disk, by initializing any/all of the tables that the file system relies on.
- **mk f** : Create file. This will create a file named f in the file system.
- **mkdir d** : Create directory. This will create a subdirectory named d in the current directory.
- **rm f** : Delete file. This will delete the file named f from the current directory.
- **cd path** : Change directory. This will change the current working directory to the path. The path is in the format in Linux, which can be a relative or absolute path. When the file system starts, the initial working path is /. You need to handle . and .. .
- **rmdir d** : Delete directory. This will delete the directory named d in the current directory.
- **ls** : Directory listing. This will return a listing of the files and directories in the current directory. You are also required to output some other information, such as file size, last update time, etc.
- **cat f** : Catch file. This will read the file named f, and return the data that came from it.
- **w f l data** : Write file. This will overwrite the contents of the file named f with the l bytes of data. If the new data is longer than the data previously in the file, the file will be made longer. If the new data is shorter than the data previously in the file, the file will be truncated to the new length.
- **i f pos l data** : Insert to a file. This will insert into the file at the position before the posth character(0-indexed), with the l bytes of data. If the pos is larger than the size of the file. Just insert it at the end of the file.
- **d f pos l** : Delete in the file. This will delete contents from the pos character (0-indexed), delete
  l bytes, or till the end of the file.
- **e** : Exit the file system.



#### Work together

- Source file name: disk.c, fs.c, client.c 
- Executable file name: disk, fs, client
- Command line: ./disk [cylinders] [sector per cylinder] [track-to-track delay] [disk-storage-filename] [DiskPort] ./fs [DiskPort] [FSPort] ./client [FSPort]

Usage:

```shell
./disk <cylinders> <sector per cylinder> <track-to-track delay> <disk-storage-filename> <DiskPort>
./fs <DiskPort> <FSPort>
./client <FSPort>
```

