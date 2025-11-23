#ifndef PIPES_H
#define PIPES_H
#include <stdint.h>

// Numero maximo de pipes de kernel disponibles simultaneamente
#define MAX_PIPES 64
// Capacidad del buffer interno de cada pipe (en bytes, ring buffer)
#define PIPE_BUFFER_CAPACITY 4096
// ID reservado para teclado/STDIN global
#define KEYBOARD_PIPE_ID 0

/*
 *  (1 lector, 1 escritor por pipe)
 * - Cada pipe permite a lo sumo un proceso lector y un proceso escritor. El
 *   primer proceso que invoque read()/write() fija su rol; otros procesos
 *   intentando usar la misma dirección (read o write) obtendrán -1.
 * - pipe_write y pipe_read intentan transferir hasta 'count' bytes.
 * - Retornan inmediatamente la cantidad efectivamente transferida (puede ser < count).
 * - BLOQUEAN solo si no pueden transferir ni un solo byte al inicio (buffer lleno en write, vacio en read).
 * - Una vez que al menos 1 byte fue transferido, si el buffer se llena (write) o se vacia (read) antes de completar, retornan parcial.
 * - En caso de parametros invalidos (pipe_id fuera de rango / no en uso, punteros nulos,
 *   count == 0) retornan -1 y no realizan transferencia.
 * - Pipe 0 (KEYBOARD_PIPE_ID) es de solo escritura desde kernel. Procesos de usuario
 *   no pueden escribir con pipe_write(0,...): retorna -1. El teclado usa
 *   exclusivamente pipe_try_kernel_nonblocking_write(0, c).
 */

/* Crea una nueva pipe de kernel.
 * Retorno: id de pipe en [0..MAX_PIPES-1] si ok, o -1 si no hay lugares libres.
 */
int pipe_create(void);

/* Escribe hasta 'count' bytes en la pipe indicada (transferencia parcial permitida).
 * Bloquea solo si no puede escribir ni 1 byte porque el buffer esta lleno.
 * Retorno: cantidad escrita (>0 y <= count), o -1 si parametros invalidos.
 * NOTA: pipe_id 0 no acepta escrituras de usuario; retorna -1. Para teclado usar
 * pipe_try_kernel_nonblocking_write.
 */
int pipe_write(int pipe_id, const char *buffer, uint64_t count);

/* Lee hasta 'count' bytes desde la pipe indicada hacia 'buffer' (transferencia parcial permitida).
 * Bloquea solo si no puede leer ni 1 byte porque el buffer esta vacio.
 * Retorno: cantidad leida (>0 y <= count), o -1 si parametros invalidos.
 */
int pipe_read(int pipe_id, char *buffer, uint64_t count);

/* Escribe 1 char en la pipe indicada si hay espacio. Uso exclusivo de kernel (IRQ teclado).
 * Comportamiento: NO BLOQUEANTE.
 * Retorno: 1 si se escribio, 0 si no hay espacio, -1 si parametros invalidos.
 */
int pipe_try_kernel_nonblocking_write(int pipe_id, char c);

/* Retorna la cantidad de bytes disponibles para leer en la pipe indicada.
 * Retorno: cantidad de bytes disponibles (>=0) o -1 si el pipe_id es inválido.
 */
int pipe_available(int pipe_id);

// Estructura exportada para listar estados de pipes via syscall
typedef struct pipe_info_s {
    int id;
    int in_use;
    uint32_t size;            // bytes actualmente en buffer
    uint32_t capacity;        // capacidad total del buffer
    uint32_t readers_waiting; // 0 o 1
    uint32_t writers_waiting; // 0 o 1
} pipe_info_t;

// Lista hasta 'max' pipes en 'out'. Retorna cantidad copiada.
int pipe_list(pipe_info_t *out, int max);

#endif // PIPES_H
