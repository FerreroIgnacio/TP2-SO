#ifndef PIPES_H
#define PIPES_H
#include <stdint.h>

// Numero maximo de pipes de kernel disponibles simultaneamente
#define MAX_PIPES 64
// Capacidad del buffer interno de cada pipe (en bytes, ring buffer)
#define PIPE_BUFFER_CAPACITY 4096
// ID reservado sugerido para pipe de teclado (opcional, puede no usarse)
#define KEYBOARD_PIPE_ID 0

/*
 * Semantica (PARCIAL):
 * - pipe_write y pipe_read intentan transferir hasta 'count' bytes.
 * - Retornan inmediatamente la cantidad efectivamente transferida (puede ser < count).
 * - BLOQUEAN solo si no pueden transferir ni un solo byte al inicio (buffer lleno en write, vacio en read).
 * - Una vez que al menos 1 byte fue transferido, si el buffer se llena (write) o se vacia (read) antes de completar, retornan parcial.
 * - En caso de parametros invalidos (pipe_id fuera de rango / no en uso, punteros nulos,
 *   count == 0) retornan -1 y no realizan transferencia.
 */

/*
 * Crea una nueva pipe de kernel.
 * Retorno: id de pipe en [0..MAX_PIPES-1] si ok, o -1 si no hay lugares libres.
 */
int pipe_create(void);

/*
 * Escribe hasta 'count' bytes en la pipe indicada (transferencia parcial permitida).
 * Bloquea solo si no puede escribir ni 1 byte porque el buffer esta lleno.
 * Retorno: cantidad escrita (>0 y <= count), o -1 si parametros invalidos.
 */
int pipe_write(int pipe_id, const char *buffer, uint64_t count);

/*
 * Lee hasta 'count' bytes desde la pipe indicada hacia 'buffer' (transferencia parcial permitida).
 * Bloquea solo si no puede leer ni 1 byte porque el buffer esta vacio.
 * Retorno: cantidad leida (>0 y <= count), o -1 si parametros invalidos.
 */
int pipe_read(int pipe_id, char *buffer, uint64_t count);

/*
 * Escribe 1 char en la pipe indicada si hay espacio.
 * Comportamiento: NO BLOQUEANTE.
 * Retorno: 1 si se escribio, 0 si no hay espacio, -1 si parametros invalidos.
 */
int pipe_try_kernel_nonblocking_write(int pipe_id, char c);

#endif // PIPES_H
