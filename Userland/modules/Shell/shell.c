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
#include "./shell_commands.h"

// Variables globales (buffer de linea ahora reside en FD dinamico shell_cmd_fd)
frameBuffer frame;
int buffer_pos = 0; // longitud actual de la linea en FD shell_cmd_fd
int cursor_x = 0;
int cursor_y = 0;
static char firstEntry = 1;
int shell_cmd_fd = -1; // FD dinamico usado como buffer de linea (>=3), expuesto para clear_buffer

// Prototipos
static void execute_command_line(const char *line);
static void handle_stdin_chunk();
static void rebuild_line_visual();
static int read_fd_size(int fd);
static int read_fd_snapshot(int fd, unsigned char *buf, int max);

// Ejecutar comando dado un string completo
static void execute_command_line(const char *line){
    hide_cursor();
    if(line==NULL || *line=='\0') return;
    // Borro cursor visual
    frameDrawChar(frame, ' ', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y);
    // Copiar linea local para parseo (pipeline etc.)
    char work[BUFFER_SIZE]; work[0]='\0';
    strncpy(work,line,BUFFER_SIZE-1); work[BUFFER_SIZE-1]='\0';
    // Detectar pipeline
    char *bar=NULL; for(char *p=work; *p; ++p){ if(*p=='|'){ bar=p; break; } }
    if(bar){
        *bar='\0';
        char *left=work; char *right=bar+1;
        while(*left==' ') left++; while(*right==' ') right++;
        char *lend=left+strlen(left); while(lend>left && lend[-1]==' ') lend--; *lend='\0';
        char *rend=right+strlen(right); while(rend>right && rend[-1]==' ') rend--; *rend='\0';
        if(*left=='\0' || *right=='\0'){ shell_print_colored("Error de sintaxis en pipeline\n", ERROR_COLOR); return; }
        char left_copy[BUFFER_SIZE]; strncpy(left_copy,left,BUFFER_SIZE-1); left_copy[BUFFER_SIZE-1]='\0';
        char right_copy[BUFFER_SIZE]; strncpy(right_copy,right,BUFFER_SIZE-1); right_copy[BUFFER_SIZE-1]='\0';
        char *left_args = find_args(left_copy);
        char *right_args = find_args(right_copy);
        int pipe_id = pipe_create(); if(pipe_id<0){ shell_print_colored("No se pudo crear pipe\n", ERROR_COLOR); return; }
        int pid_right = shell_launch_program(right_copy,right_args);
        if(pid_right<0){ shell_print_colored("Programa der. desconocido\n", ERROR_COLOR); return; }
        fd_bind_std(pid_right,0,pipe_id);
        if(!strcmp(left_copy,"echo")){
            extern int echo_proc(void*);
            char *dup=NULL; if(left_args && *left_args){ size_t len=strlen(left_args); dup=malloc(len+1); if(dup) memcpy(dup,left_args,len+1); }
            int pid_echo=new_proc((task_fn_t)echo_proc,dup);
            if(pid_echo<0){ shell_print_colored("No se pudo lanzar echo\n", ERROR_COLOR); if(dup) free(dup); return; }
            fd_bind_std(pid_echo,1,pipe_id);
        } else {
            int pid_left = shell_launch_program(left_copy,left_args);
            if(pid_left<0){ shell_print_colored("Programa izq. desconocido\n", ERROR_COLOR); return; }
            fd_bind_std(pid_left,1,pipe_id);
        }
        return;
    }
    char cmd_copy[BUFFER_SIZE]; strncpy(cmd_copy,line,BUFFER_SIZE-1); cmd_copy[BUFFER_SIZE-1]='\0';
    char *args = find_args(cmd_copy);
    command_switch(cmd_copy,args);
}

// Reconstruye visualmente la linea actual leyendo FD 3
static void rebuild_line_visual(){
    font_info_t currentFont = fontmanager_get_current_font();
    int char_width = FONT_SIZE * currentFont.width;
    int saved_y = cursor_y;
    for(int x=0; x < frameGetWidth(frame); x += char_width){ frameDrawChar(frame,' ', SHELL_COLOR, SHELL_COLOR, x, saved_y); }
    cursor_x = 0; cursor_y = saved_y;
    shell_print_colored("> ", PROMPT_COLOR);
    unsigned char temp[BUFFER_SIZE];
    int total = read_fd_snapshot(shell_cmd_fd, temp, BUFFER_SIZE-1);
    if(total > 0){
        // Restaurar contenido porque snapshot consumió los bytes
        write(shell_cmd_fd,(char*)temp,total);
        buffer_pos = total; temp[total]='\0';
        for(int i=0;i<total;i++){ shell_putchar(temp[i]); }
    } else {
        buffer_pos = 0;
    }
    reset_cursor();
}

// Maneja lectura parcial desde STDIN y actualiza FD 3
static void handle_stdin_chunk(){
    update_cursor();
    unsigned char inbuf[64]; int n = read(STDIN,inbuf,sizeof(inbuf));
    if(n <= 0) return;
    for(int i=0;i<n;i++){
        unsigned char c = inbuf[i];
        if(c=='\n'){
            unsigned char linebuf[BUFFER_SIZE]; int total = read_fd_snapshot(shell_cmd_fd, linebuf, BUFFER_SIZE-1);
            if(total>0){ linebuf[total]='\0'; }
            else { linebuf[0]='\0'; }
            buffer_pos = 0; // FD quedo vacio
            shell_newline();
            execute_command_line((char*)linebuf);
            shell_print_prompt();
            reset_cursor();
            continue;
        }
        if(c=='\b'){
            if(buffer_pos>0){
                unsigned char temp[BUFFER_SIZE]; int total = read_fd_snapshot(shell_cmd_fd, temp, BUFFER_SIZE-1);
                if(total>0){ total--; }
                // Quitar flush: ya consumimos el contenido, solo reescribir truncado
                if(total>0){ write(shell_cmd_fd,(char*)temp,total); }
                buffer_pos = total;
                rebuild_line_visual();
            }
            continue;
        }
        if(buffer_pos < BUFFER_SIZE-1){
            write(shell_cmd_fd,(char*)&c,1);
            buffer_pos++;
            shell_putchar(c);
            reset_cursor();
        }
    }
}

static void shell_welcome(){
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print_colored("             SHELL\n", PROMPT_COLOR);
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
}

int main(){
    if(firstEntry){
        frame = getFB(); if(!frame) return -1;
        clear_screen(); fontmanager_set_font(1); shell_welcome(); shell_print_prompt();
        set_priority(getpid(),0);
        shell_cmd_fd = fd_open("shellcmd"); if(shell_cmd_fd < 0){ shell_print_colored("Error creando FD shellcmd\n", ERROR_COLOR); }
        firstEntry = 0; buffer_pos = 0;
    }
    while(1){
        setFB(frame);
        handle_stdin_chunk();
    }
    return 0;
}

static int read_fd_size(int fd){
    fd_info_u_t infos[32];
    int n = fd_list(infos,32);
    if(n <= 0) return 0;
    for(int i=0;i<n;i++){ if(infos[i].fd == fd) return infos[i].size; }
    return 0;
}
static int read_fd_snapshot(int fd, unsigned char *buf, int max){
    int sz = read_fd_size(fd);
    if(sz <= 0) return 0;
    if(sz > max) sz = max;
    int r = read(fd, buf, sz);
    if(r < 0) return 0;
    return r;
}
