#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../../libs/standard/standard.h"

int cmd_createfd_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    int fd = fd_open(argv[0]);
    if (fd < 0)
    {
        printf("Error: no se pudo crear FD '%s'\n", argv[0]);
        return -1;
    }
    printf("FD creado: %d\n", fd);
    return 0;
}
