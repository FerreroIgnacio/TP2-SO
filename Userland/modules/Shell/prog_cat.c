#include "../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../libs/process/process.h"

int cat_proc(void *argv)
{
    (void)argv;
    unsigned char buf[256];
    while (1)
    {
        int n = read(STDIN, buf, sizeof(buf));
        if (n > 0)
        {
            write(STDOUT, (const char *)buf, (unsigned long)n);
        }
        else
        {
            sleep(1); // evitar busy-wait si no hay datos
        }
    }
    return 0;
}

