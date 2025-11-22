#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../../libs/standard/standard.h"

int cmd_readfd_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    int fd = strtoint(argv[0]);
    unsigned char buf[256];
    int total = 0;
    while (1)
    {
        int n = read(fd, buf, sizeof(buf) - 1);
        if (n <= 0)
            break;
        buf[n] = '\0';
        shell_print((char *)buf);
        total += n;
    }
    printf(total == 0 ? "(sin datos)\n" : "\nLeidos %d bytes de fd %d\n", total, fd);
    return 0;
}
