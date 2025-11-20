#include "./shell_commands.h"
#include "../../libs/standard/standard.h"
#include "../../libs/video/video.h"
#include "../../libs/fontManager/fontManager.h"
#include "../../libs/invalidOpCode/invalidOpCode.h"
#include "../../libs/test/test.h"
#include "../../libs/mystring/mystring.h"
#include "../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../libs/memory/memory.h"
#include "../../libs/keyboard/keyboard.h"
#include "../../libs/time/time.h"
#include "../../libs/process/process.h"
#include "../../libs/semaphores/semaphores.h"
#include "./shell_defs.h"
#include "./shell_render.h"

// Forward declarations of shell print helpers to avoid implicit declaration warnings
static void sp_print(const char *s);
static void sp_uint(uint64_t v);
static void sp_dec(int64_t v);
static void sp_hex(uint64_t v);
// Synchronous execution helper: crea proceso y espera a que termine
static void run_sync(task_fn_t fn, void *arg, int free_after){
    pid_t pid = new_proc(fn, arg);
    if (pid >= 0) {
        int st;
        waitpid(pid, &st, WHANG);
    }
    if (free_after && arg) {
        free(arg);
    }
}

#define MAX_PROCESS 100

static void *const pongisgolfModuleAddress = (void *)0x8000000;

// Comandos disponibles

// CMD_HELP DE ARQUITECTURA DE COMPUTADORAS
/*
void cmd_help()
{
    printf("Comandos disponibles:\n");
    printf("  help             - Mostrar comandos disponibles\n");
    printf("  clear            - Limpiar pantalla\n");
    printf("  echo <message>    - Mostrar texto en pantalla\n");
    printf("  datetime         - Mostrar fecha y hora UTC-0 \n");
    printf("  registers        - Imprimir registros guardados (F1)\n");
    printf("  listfonts        - Listar las fuentes disponibles\n");
    printf("  setfont <id>     - Cambiar la fuente\n");
    printf("  testzerodiv      - Testea la excepcion 00 \n");
    printf("  testinvalidcode  - Testea la excepcion 06 \n");
    printf("\nProgramas disponibles:\n");
    printf("  pongisgolf\n");
    printf("\nControles:\n");
    printf("  Enter - Ejecutar comando\n");
    printf("  Backspace - Borrar caracter\n");
}*/

int cmd_help()
{
    sp_print("Comandos disponibles:\n");
    sp_print("  help             - Mostrar comandos disponibles\n");
    sp_print("  clear            - Limpiar pantalla\n");
    sp_print("  mem              - Imprime el estado de la memoria\n");
    sp_print("  ps               - Imprime la lista de todos los procesos\n");
    sp_print("  loop <segundos>  - Imprime su ID con un saludo cada cantidad de segundos\n");
    sp_print("  kill <pid>       - Mata un proceso dado su ID\n");
    sp_print("  nice <pid> <pri> - Cambia la prioridad de un proceso\n");
    sp_print("  block <pid>      - Alterna entre ready y blocked un proceso\n");
    sp_print("  cat              - Imprime el stdin tal como lo recibe\n");
    sp_print("  wc               - Cuenta lineas del input\n");
    sp_print("  filter           - Filtra vocales del input\n");
    sp_print("  mvar             - Multiple readers problem\n");
    sp_print("  createfd <name>  - Crea un FD dinamico\n");
    sp_print("  writefd <fd> <texto> - Escribe texto en un FD dinamico\n");
    sp_print("  readfd <fd>      - Lee contenido de un FD dinamico\n");
    sp_print("  fdlist           - Lista FDs dinamicos\n");
    sp_print("\nTests disponibles:\n");
    sp_print("  test_mm <max-bytes>                     - Stress test memoria\n");
    sp_print("  test_processes <max-processes>          - Opera procesos aleatoriamente\n");
    sp_print("  test_priority <end-val-for-process>     - Prueba prioridades\n");
    sp_print("  test_synchro <processes> <inc-dec>      - Modifica variable con semaforos\n");
    sp_print("  test_no_synchro <processes> <inc-dec>   - Modifica variable sin semaforos\n");
    sp_print("\nControles:\n  Enter - Ejecutar comando\n  Backspace - Borrar caracter\n");
    exit(0);
    return 0;
}

void cmd_clear()
{
    clear_screen();
}

int cmd_mem()
{
    size_t total, used, freeMem;
    getMemInfo(&total, &used, &freeMem);
    sp_print("Estado de la memoria:\nTOTAL: "); sp_dec((int)total); sp_print("   USADO: "); sp_dec((int)used); sp_print("   LIBRE: "); sp_dec((int)freeMem); sp_print("\n");
    exit(0);
    return 0;
}

static char *dup_args(const char *src){
    if(src==NULL) return NULL;
    while(*src==' ') src++; // trim leading spaces
    int len=0; const char *p=src; while(*p && *p!='\n') { len++; p++; }
    char *d = (char*)malloc(len+1);
    if(!d) return NULL;
    for(int i=0;i<len;i++) d[i]=src[i];
    d[len]='\0';
    return d;
}

int cmd_loop(void *argv)
{
    char *arg = (char*)argv; int segs;
    if(arg==NULL || *arg=='\0'){ sp_print("Uso: loop <segundos>\n"); exit(0); }
    segs = (int)strtoint(arg);
    if(segs <= 0){ sp_print("Uso: loop <segundos>\n"); exit(0); }
    while (1){ sp_print("Hola! soy el proceso: "); sp_dec(getpid()); sp_print(". Este mensaje aparecera cada "); sp_dec(segs); sp_print(" segundos \n"); sleep(segs); }
    return 0;
}

int cmd_ps()
{
    proc_info_t proc_list[MAX_PROCESS];
    int count = get_proc_list(proc_list, MAX_PROCESS);
    for (int i = 0; i < count; i++){
        proc_info_t *p = &proc_list[i];
        sp_print("PID:"); sp_dec(p->pid); sp_print(" | Father:"); sp_dec(p->father_pid);
        sp_print(" | Pri:"); sp_dec(p->priority); sp_print(" | Ready:"); sp_dec(p->ready);
        sp_print(" | Wait:"); sp_dec(p->waiting); sp_print(" | Zombie:"); sp_dec(p->is_zombie);
        sp_print(" | Status:"); sp_dec(p->status); sp_print("\n");
    }
    exit(0);
    return 0;
}

void cmd_kill(pid_t pid){ int status = kill(pid); sp_print("Kill a proceso: "); sp_dec(pid); sp_print(" termino con estado: "); sp_dec(status); sp_print(" \n"); }
void cmd_nice(pid_t pid, process_priority_t prio){ sp_print("cambiando la prioridad del proceso: "); sp_dec(pid); sp_print(" a "); sp_dec(prio); set_priority(pid, prio); }
void cmd_block(pid_t pid){ if (block_proc(pid) == 0){ sp_print("bloqueando el proceso: "); sp_dec(pid); return; } sp_print("desbloqueando el proceso: "); sp_dec(pid); unblock_proc(pid); }

int cmd_testMM(void *argv){
    char *args = (char*)argv;
    const char *usage = "Uso: test_mm <bytes>";
    if (!args || *args == '\0') {
        sp_print(usage); sp_print("\n"); exit(1);
    }
    while (*args == ' ') args++;
    if (*args == '\0') { sp_print(usage); sp_print("\n"); exit(1); }
    char *end = args;
    while (*end && *end != ' ') end++;
    char saved = *end;
    *end = '\0';
    if (*args == '\0') { *end = saved; sp_print(usage); sp_print("\n"); exit(1); }
    if (saved != '\0') {
        char *extra = end + 1;
        while (*extra == ' ') extra++;
        if (*extra != '\0') { *end = saved; sp_print(usage); sp_print("\n"); exit(1); }
    }
    sp_print("Iniciando testMM con "); sp_print(args); sp_print(" bytes\n");
    char *test_args[] = { args };
    uint64_t result = test_mm(1, test_args);
    sp_print("testMM finalizado con codigo "); sp_hex(result); sp_print("\n");
    *end = saved;
    exit((int)result);
    return 0;
}

int cmd_testProcesses(void *argv){
    char *args = (char*)argv;
    const char *usage_msg = "Uso: test_processes <max-processes>";
    if (!args) { sp_print(usage_msg); sp_print("\n"); return 1; }
    while (*args == ' ') args++;
    if (*args == '\0') { sp_print(usage_msg); sp_print("\n"); return 1; }
    char *arg_end = args;
    while (*arg_end && *arg_end != ' ') arg_end++;
    char saved = *arg_end;
    *arg_end = '\0';
    if (*args == '\0') { sp_print(usage_msg); sp_print("\n"); *arg_end = saved; return 1; }
    if (saved != '\0') {
        char *extra = arg_end + 1;
        while (*extra == ' ') extra++;
        if (*extra != '\0') { sp_print(usage_msg); sp_print("\n"); *arg_end = saved; return 1; }
    }
    sp_print("Iniciando testProcesses con max "); sp_print(args); sp_print(" procesos...\n");
    char *test_args[] = { args };
    int64_t result = test_processes(1, test_args);
    sp_print("testProcesses finalizado con codigo "); sp_hex((uint32_t)result); sp_print("\n");
    *arg_end = saved;
    exit((int)result);
    return (int)result;
}

int cmd_testPriority(void *argv){
    (void)argv;
    sp_print("Iniciando testPriority...\n");
    char *test_args[] = { "3" };
    uint64_t result = test_prio(1, test_args);
    sp_print("testPriority finalizado con codigo "); sp_hex(result); sp_print("\n");
    return (int)result;
}
int cmd_testSynchro(void *argv){
    (void)argv;
    sp_print("Iniciando testSynchro...\n");
    uint64_t result = 0; // TODO: implementar
    sp_print("testSynchro finalizado con codigo "); sp_hex(result); sp_print("\n");
    return (int)result;
}
int cmd_testNoSynchro(void *argv){
    (void)argv;
    sp_print("Iniciando testNoSynchro...\n");
    uint64_t result = 0; // TODO: implementar
    sp_print("testNoSynchro finalizado con codigo "); sp_hex(result); sp_print("\n");
    return (int)result;
}

// ARQUI

// Proceso echo para uso en pipeline: escribe argv a STDOUT (pipe) y retorna
int echo_proc(void *argv) {
    char *arg=(char*)argv;
    if (arg && *arg) {
        write(STDOUT,arg,strlen(arg));
    }
    write(STDOUT,"\n",1);
    if(arg) free(arg);
    return 0;
}

void cmd_echo(char *args)
{
    if (args && *args) {
        // Standalone builtin: imprimir directo al framebuffer
        shell_print(args);
        shell_print("\n");
    } else {
        shell_newline();
    }
}

void cmd_dateTime(){
    uint8_t year=0, month=0, day=0, h=0, m=0, s=0;
    getLocalTime(&h,&m,&s);
    getLocalDate(&year,&month,&day);
    sp_print("Fecha y hora en UTC-0: ");
    sp_dec(day); sp_print("/"); sp_dec(month); sp_print("/20"); sp_dec(year); sp_print(" ");
    sp_dec(h); sp_print(":"); sp_dec(m); sp_print(":"); sp_dec(s); sp_print("\n");
}

void cmd_registers(){
    registers_t regs; getRegisters(&regs);
    sp_print("RIP:    0x"); sp_hex(regs.rip); sp_print("\nRFLAGS: 0x"); sp_hex(regs.rflags);
    sp_print("\nRSP:    0x"); sp_hex(regs.rsp); sp_print("\nRBP:    0x"); sp_hex(regs.rbp);
    sp_print("\nRAX:    0x"); sp_hex(regs.rax); sp_print("\nRBX:    0x"); sp_hex(regs.rbx);
    sp_print("\nRCX:    0x"); sp_hex(regs.rcx); sp_print("\nRDX:    0x"); sp_hex(regs.rdx);
    sp_print("\nRSI:    0x"); sp_hex(regs.rsi); sp_print("\nRDI:    0x"); sp_hex(regs.rdi);
    sp_print("\nR8 :    0x"); sp_hex(regs.r8); sp_print("\nR9 :    0x"); sp_hex(regs.r9);
    sp_print("\nR10:    0x"); sp_hex(regs.r10); sp_print("\nR11:    0x"); sp_hex(regs.r11);
    sp_print("\nR12:    0x"); sp_hex(regs.r12); sp_print("\nR13:    0x"); sp_hex(regs.r13);
    sp_print("\nR14:    0x"); sp_hex(regs.r14); sp_print("\nR15:    0x"); sp_hex(regs.r15); sp_print("\n");
}

void cmd_test0Div()
{
    int x = 1;
    int y = 0;
    int z = x / y;
    z++;
}

void cmd_createfd(char *args){
    if (!args || *args == '\0') { sp_print("Uso: createfd <name>\n"); return; }
    int fd = fd_open(args);
    if (fd < 0) { sp_print("Error: no se pudo crear FD '"); sp_print(args); sp_print("'\n"); return; }
    sp_print("FD creado: "); sp_dec(fd); sp_print("\n");
}
void cmd_writefd(char *args){
    if (!args || *args == '\0') { sp_print("Uso: writefd <fd> <texto>\n"); return; }
    char *fd_str = args;
    char *msg = find_args(args);
    if (!msg || *msg == '\0') { sp_print("Uso: writefd <fd> <texto>\n"); return; }
    int fd = strtoint(fd_str);
    int wrote = write(fd, msg, strlen(msg));
    if (wrote < 0) { sp_print("Error: write a fd "); sp_dec(fd); sp_print("\n"); return; }
    sp_print("Escritos "); sp_dec(wrote); sp_print(" bytes en fd "); sp_dec(fd); sp_print("\n");
}
void cmd_readfd(char *args){
    if (!args || *args == '\0') { sp_print("Uso: readfd <fd>\n"); return; }
    int fd = strtoint(args);
    unsigned char buf[256];
    int total = 0;
    while (1) {
        int n = read(fd, buf, sizeof(buf) - 1);
        if (n <= 0) break;
        buf[n] = '\0';
        shell_print((char*)buf);
        total += n;
    }
    if (total == 0) {
        sp_print("(sin datos)\n");
    } else {
        sp_print("\nLeidos "); sp_dec(total); sp_print(" bytes de fd "); sp_dec(fd); sp_print("\n");
    }
}

void shell_set_font(font_type_t font_index){ int count=fontmanager_get_font_count(); if(font_index<0||font_index>=count){ shell_print_colored("Error: indice de fuente inválido\n", ERROR_COLOR); return; } cmd_clear(); fontmanager_set_font(font_index); shell_print_colored("Fuente cambiada a: ", PROMPT_COLOR); sp_print(fontmanager_get_font_name(font_index)); sp_print("\n"); shell_newline(); }
void shell_list_fonts(){
    int count = fontmanager_get_font_count();
    sp_print("Fuentes disponibles:\n");
    for (int i = 0; i < count; i++) {
        const char *name = fontmanager_get_font_name(i);
        sp_print("  Id: "); sp_dec(i); sp_print("  -  "); sp_print(name); sp_print("\n");
    }
}
void cmd_fdlist(){
    fd_info_u_t infos[32];
    int n = fd_list(infos, 32);
    if (n <= 0) { sp_print("(sin FDs dinamicos en este proceso)\n"); return; }
    sp_print("FDs dinamicos del proceso actual ("); sp_dec(n); sp_print("):\n");
    for (int i = 0; i < n; i++) {
        sp_print("  id="); sp_dec(infos[i].fd);
        sp_print(" name="); sp_print(infos[i].name);
        sp_print(" bytes="); sp_dec(infos[i].size);
        sp_print("\n");
    }
}

    static int launch_cat(char *args);

int shell_launch_program(const char *name, char *args)
{
    if (name == NULL || *name == '\0') return -1;
    if (!strcmp(name, "cat"))
    {
        return launch_cat(args);
    }
    // otros programas externos en el futuro
    return -1;
}

extern int cat_proc(void *argv);
static int launch_cat(char *args)
{
    (void)args;
    return new_proc(cat_proc, NULL);
}

void command_switch(char *cmd_copy, char *args)
{
    if (!strcmp(cmd_copy, "help"))
    {
        run_sync((task_fn_t)cmd_help, NULL, 0);
    }
    else if (!strcmp(cmd_copy, "clear"))
    {
        cmd_clear();
    }
    else if (!strcmp(cmd_copy, "mem"))
    {
        run_sync(cmd_mem, NULL, 0);
    }
    else if (!strcmp(cmd_copy, "ps"))
    {
        run_sync(cmd_ps, NULL, 0);
    }
    else if (!strcmp(cmd_copy, "loop"))
    {
        // loop es infinito, se lanza asincronamente
        char *copy = dup_args(args);
        new_proc((task_fn_t)cmd_loop, (void *)copy);
    }
    else if (!strcmp(cmd_copy, "kill"))
    {
        cmd_kill(strtoint(args));
    }
    else if (!strcmp(cmd_copy, "nice"))
    {
        if(!args){ sp_print("Uso: nice <pid> <pri>\n"); }
        else {
            char *copy = dup_args(args);
            char *pri = find_args(copy);
            if(!pri || *pri=='\0'){ sp_print("Uso: nice <pid> <pri>\n"); }
            else { pid_t pid = strtoint(copy); process_priority_t pr = (process_priority_t)strtoint(pri); cmd_nice(pid, pr); }
            free(copy);
        }
    }
    else if (!strcmp(cmd_copy, "block"))
    {
        cmd_block(strtoint(args));
    }
    else if (!strcmp(cmd_copy, "test_mm"))
    {
        char *copy = dup_args(args);
        run_sync(cmd_testMM, copy, 1);
    }
    else if (!strcmp(cmd_copy, "test_processes"))
    {
        char *copy = dup_args(args);
        run_sync(cmd_testProcesses, copy, 1);
    }
    else if (!strcmp(cmd_copy, "test_priority"))
    {
        char *copy = dup_args(args);
        run_sync(cmd_testPriority, copy, 1);
    }
    else if (!strcmp(cmd_copy, "test_synchro"))
    {
        char *copy = dup_args(args);
        run_sync(cmd_testSynchro, copy, 1);
    }
    else if (!strcmp(cmd_copy, "test_no_synchro"))
    {
        char *copy = dup_args(args);
        run_sync(cmd_testNoSynchro, copy, 1);
    }

    // Comandos realizados en Arquitectura de Computadoras (ya no se muestran en "help"):
    else if (!strcmp(cmd_copy, "echo"))
    {
        cmd_echo(args);
    }
    else if (!strcmp(cmd_copy, "datetime"))
    {
        cmd_dateTime();
    }
    else if (!strcmp(cmd_copy, "registers"))
    {
        cmd_registers();
    }
    else if (!strcmp(cmd_copy, "testzerodiv"))
    {
        cmd_test0Div();
    }
    else if (!strcmp(cmd_copy, "testinvalidcode"))
    {
        testInvalidCode();
    }
    else if (!strcmp(cmd_copy, "listfonts"))
    {
        shell_list_fonts();
    }
    else if (!strcmp(cmd_copy, "setfont"))
    {
        if (*args)
        {
            int font_index = strtoint(args);
            shell_set_font(font_index);
        }
        else
        {
            shell_print_colored("Uso: setfont <indice>\n", ERROR_COLOR);
        }
    }
    else if (!strcmp(cmd_copy, "createfd"))
    {
        cmd_createfd(args);
    }
    else if (!strcmp(cmd_copy, "writefd"))
    {
        cmd_writefd(args);
    }
    else if (!strcmp(cmd_copy, "readfd"))
    {
        cmd_readfd(args);
    }
    else if (!strcmp(cmd_copy, "fdlist"))
    {
        cmd_fdlist();
    }
    else if (!strcmp(cmd_copy, "pongisgolf"))
    {
        int current_font = fontmanager_get_current_font_index();
        EntryPoint run = (EntryPoint)pongisgolfModuleAddress;
        // Ejecuta módulo independiente (retorna al salir del juego)
        if (run)
        {
            run();
        }
        fontmanager_set_font(current_font);
        cmd_clear();
        shell_print_prompt();
    }
    else if (cmd_copy[0] != '\0')
    {
        // Intentar programa externo (no builtin)
        int pid = shell_launch_program(cmd_copy, args);
        if (pid < 0)
        {
            shell_print_colored("Error: ", ERROR_COLOR);
            shell_print("Comando desconocido '");
            shell_print(cmd_copy);
            shell_print("'\n");
            shell_print("Escribe 'help' para ver comandos disponibles.\n");
        }
    }
}

// Helper print functions for shell (framebuffer based)
static void sp_print(const char *s){ if(s) shell_print(s); }
static void sp_uint(uint64_t v){ char buf[32]; int i=0; if(v==0){ shell_putchar('0'); return; } while(v){ buf[i++]='0'+(v%10); v/=10; } while(i--) shell_putchar(buf[i]); }
static void sp_dec(int64_t v){ if(v<0){ shell_putchar('-'); v=-v; } sp_uint((uint64_t)v); }
static void sp_hex(uint64_t v){ char buf[32]; int i=0; if(v==0){ shell_putchar('0'); return; } const char *d="0123456789abcdef"; while(v){ buf[i++]=d[v&0xF]; v>>=4; } while(i--) shell_putchar(buf[i]); }
