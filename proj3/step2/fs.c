#include "header.h"

#define SIZE 1024
#define FS 100
#define IS_FILE 0
#define IS_DIR 1
#define BLOCK_SIZE 256

char cmdbuf[SIZE], tempbuf[SIZE], ftemp[SIZE];
int blks, i;
char wrbuf[BLOCK_SIZE + 1];
int currdir = -1;
char *pwd[20];
int val = 1;

int dest;
int track = 10, sec = 10;

typedef struct
{
    char *fname;   // File Name
    char *content; // File content
    int pdir;      // Parent Directory
    int dir;       // Is Dir
    int stblk;     // Start Block
    int fsize;     // File Size
} FileTable;

typedef struct
{
    int fb;  // File Block
    int nxt; // Next Block
} FatTable;

FileTable *Tab;
FatTable *Fat;

void reset_fat()
{
    for (i = 0; i < 20; i++)
    {
        pwd[i] = (char *)malloc(sizeof(char) * SIZE);
        memset(pwd[i], '\0', SIZE);
    }
    strcpy(pwd[0], "root/");
    int i;
    for (i = 0; i < blks; i++)
    {
        Fat[i].fb = 0;
        Fat[i].nxt = 0;
    }
}

void reset_tab()
{
    int i;
    for (i = 0; i < FS; i++)
    {
        Tab[i].fname = (char *)malloc(sizeof(char) * SIZE);
        Tab[i].content = (char *)malloc(sizeof(char) * SIZE);
        strcpy(Tab[i].fname, "\0");
        strcpy(Tab[i].content, "\0");
        Tab[i].stblk = 0;
        Tab[i].dir = IS_FILE;
        Tab[i].fsize = 0;
        Tab[i].pdir = -1;
        currdir = -1;
    }
}

int free_block()
{
    for (int i = 2; i < blks; i++)
    {
        if (Fat[i].fb == 0)
            return i;
    }
    return 0;
}

int free_table_block()
{
    for (int i = 0; i < FS; i++)
    {
        if (Tab[i].stblk == 0)
            return i;
    }
}

int reset_fblock(char *name)
{
    for (i = 0; i < FS; i++)
    {
        if (strcmp(name, Tab[i].fname) == 0)
        {
            int st = 0, temp = 0;
            st = Tab[i].stblk;
            while (Fat[st].nxt != 1)
            {
                temp = Fat[st].nxt;
                Fat[st].nxt = 0;
                st = temp;
                Fat[st].fb = 0;
            }
            return 0;
        }
    }
    return -1;
}

int free_block_cnt()
{
    int cnt = 0;
    for (i = 2; i < blks; i++)
    {
        if (Fat[i].fb == 0)
            cnt++;
    }
    return cnt;
}

int block_cnt(int data)
{
    int bcnt = 0;
    bcnt = data / BLOCK_SIZE;
    if (data % BLOCK_SIZE != 0)
    {
        bcnt++;
    }
    return bcnt;
}

int is_file_exist(char *name)
{
    for (i = 0; i < FS; i++)
    {
        if (strcmp(name, Tab[i].fname) == 0 && currdir == Tab[i].pdir)
        {
            if (Tab[i].dir == IS_FILE)
                return i;
        }
    }
    return -1;
}

int is_dir_exist(char *name)
{
    for (i = 0; i < FS; i++)
    {
        if (strcmp(name, Tab[i].fname) == 0 && currdir == Tab[i].pdir)
        {
            if (Tab[i].dir == IS_DIR)
                return i;
        }
    }
    return -1;
}

int is_empty(int ind)
{
    for (i = 0; i < FS; i++)
    {
        if (Tab[i].pdir == ind)
            return 0;
    }
    return 1;
}

void format_file()
{
    reset_fat();
    reset_tab();
}

int create_file(char *name)
{
    int frblk = free_block();
    int frtab = free_table_block();
    // File Name Already Exists
    if (is_file_exist(name) != -1)
        return -1;
    // Disk Full. Delete Some data
    if (frblk == 0)
        return -1;

    if (Tab[frtab].stblk == 0)
    {
        strcpy(Tab[frtab].fname, name);
        Tab[frtab].fsize = 0;
        Tab[frtab].stblk = frblk;
        Fat[frblk].fb = frblk;
        Fat[frblk].nxt = 1;
        Tab[frtab].dir = IS_FILE;
        Tab[frtab].pdir = currdir;
        return 0;
    }
    return -1;
}

int delete_file(char *name)
{
    int index = 0;
    index = is_file_exist(name);
    int stblk;
    // File Name Doesn't Exists
    if (index == -1)
        return -1;
    // File Reset Block Unsuccessful
    if (reset_fblock(name) != 0)
        return -1;
    stblk = Tab[index].stblk;
    Fat[stblk].fb = 0;
    Fat[stblk].nxt = 0;
    memset(Tab[index].fname, '\0', 9);
    memset(Tab[i].content, '\0', SIZE);
    Tab[index].stblk = 0;
    Tab[index].dir = IS_FILE;
    Tab[index].pdir = -1;
    Tab[index].fsize = 0;
    return 0;
}

int list_file()
{
    char *file_order, *dir_order, *tmp;
    file_order = (char *)malloc(sizeof(char) * SIZE);
    dir_order = (char *)malloc(sizeof(char) * SIZE);
    tmp = (char *)malloc(sizeof(char) * SIZE);
    memset(tempbuf, '\0', SIZE);
    for (i = 0; i < FS; i++)
    {
        if (strcmp(Tab[i].fname, "\0") != 0 && currdir == Tab[i].pdir)
        {
            if (Tab[i].dir == IS_FILE)
            {
                sprintf(tmp, "%s ", Tab[i].fname);
                strcat(file_order, tmp);
            }
            else
            {
                sprintf(tmp, "%s ", Tab[i].fname);
                strcat(dir_order, tmp);
            }
        }
    }
    if (strcmp(file_order, "\0") == 0 && strcmp(dir_order, "\0") == 0)
        return -1;
    strcat(tempbuf, file_order);
    strcat(tempbuf, "& ");
    strcat(tempbuf, dir_order);
    tempbuf[strlen(tempbuf)] = '\n';
    return 0;
}

int read_file(char *name)
{
    int index = 0;
    index = is_file_exist(name);
    int stblk;
    memset(tempbuf, '\0', SIZE);
    // File Name Doesn't Exists
    if (index == -1)
        return -1;
    // File Empty. No data
    if (Tab[index].fsize == 0)
        return -1;
    stblk = Tab[index].stblk;
    while (1)
    {
        sprintf(tempbuf, "%s\n", Tab[index].content);
        if (Fat[stblk].nxt == 1)
            break;
        stblk = Fat[stblk].nxt;
    }
    return 0;
}

int write_file(char *name, char *data, int len)
{
    int blkcnt = 0, freebcnt = 0, index = 0, nf = 0;
    index = is_file_exist(name);
    int sfb, stblk;

    // File Name Doesn't Exists
    if (index == -1)
        return -1;

    freebcnt = free_block_cnt();
    blkcnt = block_cnt(len);
    // File Empty. No data
    if (freebcnt < blkcnt)
        return -1;
    // File Creation Unsuccessful
    if (reset_fblock(name) != 0)
        return -1;
    sfb = stblk = Tab[index].stblk;
    Tab[index].fsize = len;
    memset(Tab[index].content, '\0', SIZE);
    strncpy(Tab[index].content, data, len);
    for (i = 1; i <= blkcnt; i++)
    {
        Fat[stblk].fb = sfb;
        if (i == blkcnt)
        {
            Fat[stblk].fb = stblk;
            Fat[stblk].nxt = 1;
        }
        else
        {
            nf = free_block();
            Fat[stblk].fb = stblk;
            Fat[stblk].nxt = nf;
            stblk = nf;
        }
    }
    return 0;
}

int change_file(char *name, char *data, int pos, int len, int flag)
{
    int blkcnt = 0, freebcnt = 0, index = 0, nf = 0;
    index = is_file_exist(name);
    int sfb, stblk;
    char *tmp1, *tmp2;
    tmp1 = (char *)malloc(sizeof(char) * SIZE);
    tmp2 = (char *)malloc(sizeof(char) * SIZE);

    // File Name Doesn't Exists
    if (index == -1)
        return -1;

    if (flag)
    {
        strncpy(tmp2, data, len);
        strncpy(tmp1, Tab[index].content, pos);
        strcat(tmp1, tmp2);
        strcat(tmp1, Tab[index].content + pos);
        len = strlen(tmp1);
    }
    else
    {
        strncpy(tmp1, Tab[index].content, pos);
        if (strlen(Tab[index].content) < pos + len)
            len = strlen(tmp1);
        else
        {
            strcat(tmp1, Tab[index].content + pos + len);
            len = strlen(tmp1);
        }
    }

    freebcnt = free_block_cnt();
    blkcnt = block_cnt(len);
    // File Empty. No data
    if (freebcnt < blkcnt)
        return -1;
    // File Creation Unsuccessful
    if (reset_fblock(name) != 0)
        return -1;

    sfb = stblk = Tab[index].stblk;
    Tab[index].fsize = len;
    memset(Tab[index].content, '\0', SIZE);
    strncpy(Tab[index].content, tmp1, len);
    for (i = 1; i <= blkcnt; i++)
    {
        Fat[stblk].fb = sfb;
        if (i == blkcnt)
        {
            Fat[stblk].fb = stblk;
            Fat[stblk].nxt = 1;
        }
        else
        {
            nf = free_block();
            Fat[stblk].fb = stblk;
            Fat[stblk].nxt = nf;
            stblk = nf;
        }
    }
    return 0;
}

int create_dir(char *name)
{
    int i;
    int frblk = free_block();
    int frtab = free_table_block();
    int index = is_dir_exist(name);

    // Directory Name Already Exists
    if (index != -1 && (Tab[index].dir == 1))
        return -1;
    // Disk Full. Delete Some data
    if (frblk == 0)
        return -1;

    if (Tab[frtab].stblk == 0)
    {
        strcpy(Tab[frtab].fname, name);
        Tab[frtab].fsize = 0;
        Tab[frtab].stblk = frblk;
        Fat[frblk].fb = frblk;
        Fat[frblk].nxt = 1;
        Tab[frtab].dir = IS_DIR;
        Tab[frtab].pdir = currdir;
        return 0;
    }
    return -1;
}

int change_dir(char *name)
{

    int index = is_dir_exist(name);
    if (strcmp(name, "..") == 0)
    {
        // You are currently in the root Directory
        if (currdir == -1)
            return -1;
        else
        {
            if (val != 1)
            {
                val--;
                memset(pwd[val], '\0', 10);
            }
            currdir = Tab[currdir].pdir;
            return 0;
        }
    }
    // Directory Name Doesn't Exists
    else if (index == -1)
        return -1;
    else
    {
        strcpy(pwd[val], name);
        strcat(pwd[val], "/");
        val++;
        currdir = index;
        return 0;
    }
}

int delete_dir(char *name)
{
    int index = 0;
    index = is_dir_exist(name);
    int stblk, res;
    res = is_empty(index);
    // Directory Name Doesn't Exists
    if (index == -1)
        return -1;
    // Directory is not Empty
    if (res == 0)
        return -1;
    stblk = Tab[index].stblk;
    Fat[stblk].fb = 0;
    Fat[stblk].nxt = 0;
    memset(Tab[index].fname, '\0', 9);
    memset(Tab[index].content, '\0', SIZE);
    Tab[index].stblk = 0;
    Tab[index].dir = IS_FILE;
    Tab[index].pdir = -1;
    Tab[index].fsize = 0;
    return 0;
}

int main(int argc, char *argv[])
{

    char *cmd, *fname;
    int len1, len2, num, count = 0, cmdID = 0;

    char ch;
    char *info, *exitInfo;

    cmd = (char *)malloc(sizeof(char) * SIZE);
    fname = (char *)malloc(sizeof(char) * SIZE);
    info = (char *)malloc(sizeof(char) * SIZE);
    exitInfo = (char *)malloc(sizeof(char) * SIZE);
    strcpy(exitInfo, "Goodbye!\n");

    if (argc != 1)
    {
        fprintf(stderr, "Usage: ./fs \n");
        exit(1);
    }

    Tab = (FileTable *)malloc(sizeof(FileTable) * FS);
    int TabSize = sizeof(FileTable) * FS;
    reset_tab();
    blks = track * sec;
    Fat = (FatTable *)malloc(sizeof(FatTable) * blks);

    dest = open("fs.log", O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (dest == -1)
    {
        perror("Error opening file");
        exit(1);
    }

    reset_fat();

    printf("Input here:\n");

    while (1)
    {
        memset(cmdbuf, '\0', SIZE);
        memset(cmd, '\0', SIZE);
        memset(info, '\0', SIZE);
        memset(tempbuf, '\0', SIZE);
        memset(fname, '\0', SIZE);
        len1 = len2 = num = -1;
        fgets(cmdbuf, SIZE, stdin);

        ch = toupper(cmdbuf[0]);
        if (ch == 'W')
            sscanf(cmdbuf, "%s %s %d %n", cmd, fname, &len1, &num);
        else if (ch == 'D')
            sscanf(cmdbuf, "%s %s %d %d", cmd, fname, &len1, &len2);
        else
            sscanf(cmdbuf, "%s %s %d %d %n", cmd, fname, &len1, &len2, &num);

        if (strcasecmp(cmd, "f") == 0)
            cmdID = 1;
        else if (strcasecmp(cmd, "mk") == 0)
            cmdID = 2;
        else if (strcasecmp(cmd, "rm") == 0)
            cmdID = 3;
        else if (strcasecmp(cmd, "ls") == 0)
            cmdID = 4;
        else if (strcasecmp(cmd, "cat") == 0)
            cmdID = 5;
        else if (strcasecmp(cmd, "w") == 0)
            cmdID = 6;
        else if (strcasecmp(cmd, "mkdir") == 0)
            cmdID = 7;
        else if (strcasecmp(cmd, "cd") == 0)
            cmdID = 8;
        else if (strcasecmp(cmd, "pwd") == 0)
            cmdID = 9;
        else if (strcasecmp(cmd, "rmdir") == 0)
            cmdID = 10;
        else if (strcasecmp(cmd, "i") == 0)
            cmdID = 11;
        else if (strcasecmp(cmd, "d") == 0)
            cmdID = 12;
        else if (strcasecmp(cmd, "e") == 0)
            cmdID = 13;
        else
            cmdID = 0;
        switch (cmdID)
        {
        // File Format Case
        case 1:
            format_file();
            sprintf(info, "%s", "Done\n");
            write(dest, info, strlen(info));
            break;
        // File Create Case
        case 2:
            if (strcmp(fname, "\0") > 0 && create_file(fname) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // File Delete case
        case 3:
            if (strcmp(fname, "\0") > 0 && delete_file(fname) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // File List Case
        case 4:
            if (list_file() == 0)
                write(dest, tempbuf, strlen(tempbuf));
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // File Read Case
        case 5:
            if (strcmp(fname, "\0") > 0 && read_file(fname) == 0)
                write(dest, tempbuf, strlen(tempbuf));
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // File Write case
        case 6:
            memcpy(tempbuf, cmdbuf + num, len1);
            if (tempbuf[strlen(tempbuf) - 1] == '\n')
                tempbuf[strlen(tempbuf) - 1] = '\0';
            if (strcmp(fname, "\0") > 0 && len1 > 0 && num > 0 && write_file(fname, tempbuf, len1) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {

                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // Makedir Case
        case 7:
            if (strcmp(fname, "\0") > 0 && create_dir(fname) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // Change Directory Case
        case 8:
            if (strcmp(fname, "\0") > 0 && change_dir(fname) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // Present Working Directory Case
        case 9:
            memset(ftemp, '\0', SIZE);
            strcat(ftemp, pwd[0]);
            for (i = 1; i < 20; i++)
                strcat(ftemp, pwd[i]);
            ftemp[strlen(ftemp)] = '\n';
            write(dest, ftemp, strlen(ftemp));
            break;
        // Remove Directory Case
        case 10:
            if (strcmp(fname, "\0") > 0 && delete_dir(fname) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // Insert to File Case
        case 11:
            memcpy(tempbuf, cmdbuf + num, len2);
            if (tempbuf[strlen(tempbuf) - 1] == '\n')
                tempbuf[strlen(tempbuf) - 1] = '\0';
            if (strcmp(fname, "\0") > 0 && len1 >= 0 && len2 > 0 && num > 0 && change_file(fname, tempbuf, len1, len2, 1) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // Delete in File Case
        case 12:
            if (strcmp(fname, "\0") > 0 && len1 >= 0 && len2 > 0 && change_file(fname, tempbuf, len1, len2, 0) == 0)
            {
                sprintf(info, "%s", "Yes\n");
                write(dest, info, strlen(info));
            }
            else
            {
                sprintf(info, "%s", "No\n");
                write(dest, info, strlen(info));
            }
            break;
        // Exit case
        case 13:
            write(dest, exitInfo, strlen(exitInfo));
            close(dest);
            exit(1);
        default:
            printf("InCorrect Command\n");
            break;
        }
    }
    close(dest);
    exit(0);
}
