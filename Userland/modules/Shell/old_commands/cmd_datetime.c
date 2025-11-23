#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/time/time.h"

int cmd_datetime_run(int argc, char **argv)
{
    uint8_t year = 0, month = 0, day = 0, h = 0, m = 0, s = 0;
    getLocalTime(&h, &m, &s);
    getLocalDate(&year, &month, &day);
    printf("Fecha y hora en UTC-0: %d/%d/20%d %d:%d:%d\n", day, month, year, h, m, s);
    exit(0);
    return 0;
}
