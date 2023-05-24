#include "header.h"

#define SIZE 1024
#define h_addr h_addr_list[0]
char buf[SIZE];

int main(int argc, char *argv[])
{
    int sockfd;
    int nread;
    struct sockaddr_in serv_addr;
    struct hostent *host;
    uint16_t serv_port;
    char *ip;
    int i;

    if (argc != 2)
    {
        fprintf(stderr, "\nUsage: /.client <PORT>\n\n");
        exit(1);
    }
    serv_port = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    host = gethostbyname("localhost");
    memcpy(&serv_addr.sin_addr.s_addr, host->h_addr, host->h_length);
    serv_addr.sin_port = htons(serv_port);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror(NULL);
        exit(1);
    }

    char str[SIZE];
    printf("Enter your Command: ");
    while (fgets(str, SIZE, stdin) != NULL)
    {
        if (strcmp(str, "\n") == 0)
        {
            printf("Enter your Command: ");
            continue;
        }
        nread = write(sockfd, str, strlen(str));
        nread = read(sockfd, buf, SIZE);
        for (i = 1; i < nread; i++)
        {
            if (buf[i] == 0)
                buf[i] = '0';
        }
        printf("Receive from fs: %s", buf);
        printf("Enter your Command: ");
        if (strcasecmp(str, "e\n") == 0)
        {
            printf("Exiting Client\n");
            break;
        }
        memset(str, '\0', strlen(str));
        memset(buf, '\0', strlen(buf));
    }
    close(sockfd);
    exit(0);
}