#include "header.h"

#define SIZE 1024
#define BLOCK_SIZE 256

int track, sec, delay;
int fd;
char buf[SIZE];
char diskfile[SIZE];
char reader[BLOCK_SIZE + 10];

// block of 256 characters
struct block
{
    char val[BLOCK_SIZE];
};
struct block *blkmap;

int main(int argc, char *argv[])
{
    int sockfd, client_sockfd;
    int nread;
    socklen_t len;
    struct sockaddr_in serv_addr, client_addr;
    uint16_t serv_port;

    int result, tk, sc, num;
    char cmd;
    char *info, *exitInfo;

    info = (char *)malloc(sizeof(char) * SIZE);
    exitInfo = (char *)malloc(sizeof(char) * SIZE);
    strcpy(exitInfo, "Goodbye!\n");

    off_t readfrm = 0, writeto = 0;
    int totblk = 0, cur = 0;

    if ((argc != 6))
    {
        fprintf(stderr, "Usage: ./disk <tracks> <sector per cylinder> <track-to-track delay> <filename> <DiskPort>\n");
        exit(1);
    }

    track = atoi(argv[1]);
    sec = atoi(argv[2]);
    delay = atoi(argv[3]);
    strncpy(diskfile, argv[4], SIZE - 1);
    serv_port = atoi(argv[5]);
    long int fsize = track * sec * BLOCK_SIZE;
    totblk = track * sec;

    /*creating a file based on the given argument*/
    fd = open(diskfile, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(1);
    }
    // Using lseek to stretch the file size to the size of the simulated disk
    result = lseek(fd, fsize - 1, SEEK_SET);
    if (result == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(1);
    }
    // Write something at the end of the file to ensure the file actually have the new size.
    result = write(fd, "", 1);
    if (result != 1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(2);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(serv_port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror(NULL);
        exit(3);
    }

    listen(sockfd, 5);

    while (1)
    {
        printf("Listening...\n");
        len = sizeof(client_addr);
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
        if (client_sockfd == -1)
        {
            perror(NULL);
            continue;
        }

        while (1)
        {
            tk = sc = num = 0;
            memset(buf, '\0', strlen(buf));
            memset(info, '\0', strlen(info));
            memset(reader, '\0', strlen(reader));

            nread = read(client_sockfd, buf, SIZE);
            if (nread == 0)
            {
                printf("Closing Client\n");
                break;
            }

            buf[nread] = '\0';
            printf("Receive from fs: %s\n", buf);

            sscanf(buf, "%c %d %d %n", &cmd, &tk, &sc, &num);
            cmd = toupper(cmd);

            switch (cmd)
            {
            // Sends the information about tracks and sector
            case 'I':
                sprintf(info, "%d %d", track, sec);
                info[strlen(info)] = '\n';
                write(client_sockfd, info, strlen(info));
                break;
            // File Read Case
            case 'R':
                if (tk > 0 && sc > 0)
                {
                    // Read from Block
                    readfrm = (tk - 1) * sec + (sc - 1);
                    blkmap = (struct block *)mmap(0, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                    if (blkmap == MAP_FAILED)
                    {
                        close(fd);
                        perror("Error mmapping the file");
                        exit(EXIT_FAILURE);
                    }
                    memcpy(reader + 4, blkmap[readfrm].val, BLOCK_SIZE);

                    if (readfrm < totblk)
                    {

                        reader[0] = 'Y';
                        reader[1] = 'e';
                        reader[2] = 's';
                        reader[3] = ' ';
                        usleep(delay * abs(cur - readfrm));
                        cur = readfrm;
                    }
                    else
                    {
                        sprintf(reader, "No");
                    }
                    if (munmap(blkmap, fsize) == -1)
                    {
                        perror("Error un-mmapping the file");
                    }
                    if (strcmp(reader, "Yes ") == 0 || strcmp(reader, "No") == 0)
                        reader[strlen(reader)] = '\n';
                    write(client_sockfd, reader, strlen(reader));
                }
                else
                {
                    sprintf(info, "No\n");
                    write(client_sockfd, info, strlen(info));
                    printf("Instruction Error!\n");
                }
                break;
            // File Write case
            case 'W':
                if (tk > 0 && tk <= track && sc > 0 && sc <= sec)
                {
                    // Write in Block
                    writeto = (tk - 1) * sec + (sc - 1);
                    blkmap = (struct block *)mmap(0, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                    if (blkmap == MAP_FAILED)
                    {
                        close(fd);
                        perror("Error mmapping the file");
                        exit(EXIT_FAILURE);
                    }
                    usleep(delay * abs(cur - readfrm));
                    cur = readfrm;
                    memset(blkmap[writeto].val, '\0', BLOCK_SIZE);
                    memcpy(blkmap[writeto].val, buf + num, BLOCK_SIZE);
                    sprintf(info, "Yes\n");
                    write(client_sockfd, info, strlen(info));
                    if (munmap(blkmap, fsize) == -1)
                    {
                        perror("Error un-mmapping the file");
                    }
                }
                else
                {
                    sprintf(info, "No\n");
                    write(client_sockfd, info, strlen(info));
                    printf("Instruction Error!\n");
                }
                break;
            // Exit
            case 'E':
                write(client_sockfd, exitInfo, strlen(exitInfo));
                exit(1);
            default:
                printf("InCorrect Command\n");
            }
        }
        close(client_sockfd);
    }
}
