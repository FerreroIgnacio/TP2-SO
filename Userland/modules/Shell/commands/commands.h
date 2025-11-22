#ifndef SHELL_MODULE_COMMANDS_H
#define SHELL_MODULE_COMMANDS_H

#include "../../../libs/fontManager/fontManager.h"
#include "../shell_defs.h"

// Cada comando expone una función con firma uniforme
// int <nombre>_run(int argc, char **argv);
// Devuelve 0 en éxito, negativo en error de argumentos.

int cmd_help_run();
int cmd_clear_run(int argc, char **argv);
int cmd_mem_run(int argc, char **argv);
int cmd_ps_run(int argc, char **argv);
int cmd_loop_run(int argc, char **argv);
int cmd_kill_run(int argc, char **argv);
int cmd_nice_run(int argc, char **argv);
int cmd_block_run(int argc, char **argv);
int cmd_cat_run(int argc, char **argv);
int cmd_wc_run(int argc, char **argv);
int cmd_filter_run(int argc, char **argv);
int cmd_mvar_run(int argc, char **argv);
int cmd_test_mm_run(int argc, char **argv);
int cmd_test_processes_run(int argc, char **argv);
int cmd_test_priority_run(int argc, char **argv);
int cmd_test_synchro_run(int argc, char **argv);
int cmd_test_no_synchro_run(int argc, char **argv);
int cmd_echo_run(int argc, char **argv);
int cmd_datetime_run(int argc, char **argv);
int cmd_registers_run(int argc, char **argv);
int cmd_testzerodiv_run(int argc, char **argv);
int cmd_testinvalidcode_run(int argc, char **argv);
int cmd_listfonts_run(int argc, char **argv);
int cmd_setfont_run(int argc, char **argv);
int cmd_createfd_run(int argc, char **argv);
int cmd_writefd_run(int argc, char **argv);
int cmd_readfd_run(int argc, char **argv);
int cmd_fdlist_run(int argc, char **argv);
int cmd_pipelist_run(int argc, char **argv);
int cmd_pong_run(int argc, char **argv);

// Funciones auxiliares reutilizadas por comandos (expuestas para otros módulos)
void shell_set_font(font_type_t font_index);
void shell_list_fonts();
void cmd_echo(char *args); // compatibilidad con pipeline existente
int echo_proc(void *argv); // para uso en pipeline

#endif
