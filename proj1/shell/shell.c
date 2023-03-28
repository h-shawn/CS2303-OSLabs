#include "header.h"

#define BUFFER_SIZE 1024
#define MAX_ARGS 32
#define MAX_CLIENTS 10

void *handle_client(void *arg);
void execute_command(char *command, int client_socket);
void handle_pipe(char *command, int client_socket);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Port>.\n", argv[0]);
        exit(-1);
    }
    int server_socket, client_socket, status;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int port = atoi(argv[1]);

    // Create a socket for the server
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket");
        exit(1);
    }

    // Bind the socket to a port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("Server started on port %d\n", port);
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }
        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Create a new thread to handle the client request
        pthread_t thread;
        int *sock_ptr = malloc(sizeof(int));
        *sock_ptr = client_socket;

        if (pthread_create(&thread, NULL, handle_client, (void *)sock_ptr) != 0)
        {
            perror("pthread_create");
            close(client_socket);
            continue;
        }
        pthread_detach(thread);
    }
    return 0;
}

void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    int n;
    while (1)
    {
        n = write(client_socket, "shell> ", 8);
        if (n < 0)
        {
            perror("write");
            break;
        }
        // Read a command from the client
        n = read(client_socket, buffer, BUFFER_SIZE);
        if (n < 0)
        {
            perror("read");
            break;
        }
        execute_command(buffer, client_socket);
    }
}

void execute_command(char *command, int client_socket)
{
    int len = strlen(command);
    for (int i = 0; i < len; i++)
    {
        if (command[i] == '\r' || command[i] == '\n')
            command[i] = 0;
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    if (getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len) == -1)
    {
        printf("getsockname error\n");
        exit(0);
    }
    printf("received command: %s from %s:%d\n", command, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    char tmp[64];
    strcpy(tmp, command);
    char *args[MAX_ARGS];
    int num_args = 0;
    char *arg = strtok(tmp, " ");
    while (arg != NULL && num_args < MAX_ARGS - 1)
    {
        args[num_args++] = arg;
        arg = strtok(NULL, " ");
    }
    args[num_args] = NULL;

    if (strcmp(args[0], "exit") == 0)
    {
        printf("Client disconnected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        close(client_socket);
        pthread_exit(NULL);
    }

    // Count the number of pipes in the command
    int num_pipes = 0;
    for (int i = 0; i < num_args; i++)
    {
        if (strcmp(args[i], "|") == 0)
            num_pipes++;
    }

    pid_t pid = fork();

    if (pid == 0)
    {

        if (num_pipes == 0)
        {
            // No pipes, just execute the command
            dup2(client_socket, STDOUT_FILENO);
            if (execvp(args[0], args) == -1)
            {
                printf("Error: running command: '%s'\n", command);
                exit(1);
            }
        }
        else
            handle_pipe(command, client_socket), exit(0);
    }
    else if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    waitpid(pid, NULL, 0);
}

void handle_pipe(char *command, int client_socket)
{
    int mypipe[2];
    int fd_in = 0, i = 0;
    char tmp[64];
    strcpy(tmp, command);
    char *arr[MAX_ARGS][MAX_ARGS], *cmd[MAX_ARGS];

    cmd[0] = strtok(tmp, "|");
    while (cmd[i] != NULL)
        i++, cmd[i] = strtok(NULL, "|");

    i = 0;
    while (cmd[i] != NULL)
    {
        int j = 0;
        arr[i][j] = strtok(cmd[i], " ");

        while (arr[i][j] != NULL)
        {
            j++;
            arr[i][j] = strtok(NULL, " ");
        }
        if (pipe(mypipe))
        {
            fprintf(stderr, "Pipe failed.\n");
            exit(-1);
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // Redirect the output to go to the client socket
            dup2(client_socket, STDOUT_FILENO);
            dup2(fd_in, STDIN_FILENO);
            if (cmd[i + 1] != NULL)
                dup2(mypipe[1], STDOUT_FILENO);
            close(mypipe[0]);
            if (execvp(arr[i][0], arr[i]) == -1)
            {
                printf("Error: running command: '%s'\n", command);
                exit(1);
            }
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        else
        {
            wait(NULL);
            close(mypipe[1]);
            fd_in = mypipe[0];
            i++;
        }
    }
}
