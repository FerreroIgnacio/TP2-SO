#ifndef PIPES_H
#define PIPES_H
#include <stdint.h>

// Número máximo de pipes de kernel disponibles simultáneamente
#define MAX_PIPES 64
// Capacidad del buffer interno de cada pipe (en bytes, ring buffer)
#define PIPE_BUFFER_CAPACITY 4096
// ID reservado sugerido para pipe de teclado (opcional, puede no usarse)
#define KEYBOARD_PIPE_ID 0

/*
 * - pipe_write y pipe_read son BLOQUEANTES: operan hasta transferir 'count' bytes.
 * - Si el buffer está lleno (en write) o vacío (en read), la llamada cede CPU y
 *   reintenta hasta poder continuar (scheduler_yield).
 * - En caso de parámetros inválidos (pipe_id fuera de rango/no en uso, punteros nulos,
 *   count == 0) retornan -1 y no realizan ninguna transferencia.
 */

/*
 * Crea una nueva pipe de kernel.
 * Retorno: id de pipe en [0..MAX_PIPES-1] si tuvo éxito, o -1 si no hay lugares libres.
 */
int pipe_create(void);

/*
 * Escribe exactamente 'count' bytes en la pipe indicada.
 * Comportamiento: BLOQUEANTE. Si no hay espacio, espera hasta que lo haya.
 * Retorno: 'count' si fue exitoso; -1 si parámetros inválidos.
 */
int pipe_write(int pipe_id, const char *buffer, uint64_t count);

/*
 * Lee exactamente 'count' bytes desde la pipe indicada hacia 'buffer'.
 * Comportamiento: BLOQUEANTE. Si no hay datos, espera hasta que lleguen.
 * Retorno: 'count' si fue exitoso; -1 si parámetros inválidos.
 */
int pipe_read(int pipe_id, char *buffer, uint64_t count);

/*
 * Escribe 1 char en la pipe indicada si hay espacio.
 * Comportamiento: NO BLOQUEANTE. Retorna inmediatamente.
 * Retorno: 1 si se escribió el char; 0 si no hay espacio; -1 si parámetros inválidos.
 */
int pipe_try_kernel_nonblocking_write(int pipe_id, char c);

#endif // PIPES_H
