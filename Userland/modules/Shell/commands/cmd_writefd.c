#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/mystring/mystring.h"

int cmd_writefd_run(int argc, char **argv)
{
    if (argc < 2)
        return -1;
    int fd = strtoint(argv[0]);
    size_t msglen = 0;
    for (int i = 1; i < argc; i++)
        msglen += strlen(argv[i]) + 1;
    char *msg = malloc(msglen + 1);
    if (!msg)
        return -1;
    msg[0] = '\0';
    for (int i = 1; i < argc; i++)
    {
        strcat(msg, argv[i]);
        if (i + 1 < argc)
            strcat(msg, " ");
    }
    int wrote = write(fd, msg, strlen(msg));
    if (wrote < 0)
    {
        printf("Error: write a fd %d\n", fd);
        free(msg);
        return -1;
    }
    printf("Escritos %d bytes en fd %d\n", wrote, fd);
    free(msg);
    return 0;
}
