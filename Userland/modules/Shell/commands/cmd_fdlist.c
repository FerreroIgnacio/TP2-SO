#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/mystring/mystring.h" // para strtoint, strcmp
#include "../shell_render.h"

static const char *help_itoa(int n) {
    static char buf[32];
    int i = (int)sizeof(buf) - 1;
    buf[i] = '\0';
    unsigned int un = (n < 0) ? (unsigned int)(-n) : (unsigned int)n;
    do {
        buf[--i] = (char)('0' + (un % 10));
        un /= 10;
    } while (un != 0);
    if (n < 0) {
        buf[--i] = '-';
    }
    return &buf[i];
}

static int is_numeric_token(const char *s){
    if(!s || !*s) return 0;
    if(*s=='+' || *s=='-') s++;
    int hasDigit = 0;
    while(*s){
        if(*s<'0' || *s>'9') return 0;
        hasDigit = 1; s++;
    }
    return hasDigit;
}

int cmd_fdlist_run(int argc, char **argv)
{
    int targetPid = -1;
    fprintf(3, "[fdlist debug] argc=%d\n", argc);
    for(int i=0;i<argc;i++){
        if(argv[i]) fprintf(3, "[fdlist debug] argv[%d]=%s\n", i, argv[i]);
    }
    for(int i=0;i<argc;i++){
        if(argv[i] && strcmp(argv[i], "fdlist")!=0 && is_numeric_token(argv[i])){
            targetPid = strtoint(argv[i]);
            fprintf(3, "[fdlist debug] parsed pid=%d from argv[%d]\n", targetPid, i);
            break;
        }
    }

    fd_info_u_t infos[32];
    int n;
    if (targetPid < 0) {
        n = fd_list(infos, 32);
        targetPid = getpid();
        fprintf(3, "[fdlist debug] usando proceso actual pid=%d\n", targetPid);
    } else {
        n = fd_list_pid(targetPid, infos, 32);
        fprintf(3, "[fdlist debug] fd_list_pid retorno %d para pid=%d\n", n, targetPid);
    }
    if (n <= 0) {
        fprintf(3, "(sin FDs dinámicos o pid inválido en proceso %d)\n", targetPid);
        consume_render_fd();
        return 0;
    }

    fprintf(3, "FDs dinámicos del proceso %d (total=%d):\n", targetPid, n);
    shell_print_colored("FDs dinámicos del proceso ", 0x0000FF);
    shell_print_colored(help_itoa(targetPid), 0x0000FF);
    shell_print_colored("\n", 0x0000FF);

    for (int i = 0; i < n; i++) {
        fprintf(3, "  id=%d name=%s bytes=%u\n", infos[i].fd, infos[i].name, (unsigned) infos[i].size);
        shell_print_colored("id:", 0x0000FF);
        shell_print_colored(help_itoa(infos[i].fd), 0x0000FF);
        shell_print_colored(" name:", 0x0000FF);
        shell_print_colored(infos[i].name, 0x0000FF);
        shell_print_colored(" bytes:", 0x0000FF);
        shell_print_colored(help_itoa((int)infos[i].size), 0x0000FF);
        shell_print_colored("\n", 0x0000FF);
    }
    consume_render_fd();
    return 0;
}
