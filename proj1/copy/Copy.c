#include "header.h"

void copyfile(const char *input, const char *output, const int buffer_size, clock_t start)
{
    int mypipe[2];
    pid_t child_id;
    char buffer[buffer_size];

    if (pipe(mypipe))
    {
        fprintf(stderr, "Pipe failed.\n");
        exit(-1);
    }

    int src = open(input, O_RDONLY);
    int dest = open(output, O_WRONLY | O_CREAT | O_EXCL, 0777);
    if (src == -1)
    {
        printf("Error: Could not open file '%s'.\n", input);
        exit(-1);
    }
    if (dest < 0)
    {
        printf("Over write ? Y/N\n");
        char ch = getchar();
        if (ch == 'Y')
        {
            close(dest);
            dest = open(output, O_WRONLY | O_TRUNC);
        }
        else
        {
            exit(-1);
        }
    }

    child_id = fork();
    if (child_id < 0)
    {
        printf("Fork Error!");
        exit(-1);
    }
    else if (child_id > 0)
    {
        // read process: src -> pipe
        close(mypipe[0]);
        while (read(src, buffer, sizeof(buffer)) > 0)
        {
            write(mypipe[1], buffer, sizeof(buffer));
        }
        close(mypipe[1]);
        close(src);
        printf("Read file end.\n");
        wait(NULL); // wait for the execution of the child process
        clock_t end = clock();
        double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
        printf("Time used: %f millisecond\n", elapsed);
    }
    else
    {
        // write process: pipe -> dest
        close(mypipe[1]);
        while (read(mypipe[0], buffer, sizeof(buffer)) > 0)
        {
            write(dest, buffer, sizeof(buffer));
        }
        close(mypipe[0]);
        close(dest);
        printf("Write file end.\n");
    }
}

int main(int argc, const char *argv[])
{
    clock_t start = clock();

    if (argc != 4)
    {
        printf("Usage: %s <InputFile> <OutputFile> <BufferSize>.\n", argv[0]);
        exit(-1);
    }

    struct stat buffer;
    stat(argv[1], &buffer);
    if (S_ISREG(buffer.st_mode)) // check if input is a file
    {
        copyfile(argv[1], argv[2], atoi(argv[3]), start);
    }
    else
    {
        printf("There is not such a file named %s.\n", argv[1]);
        exit(-1);
    }
}