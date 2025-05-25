
#include <syscalls.h>


int syscallHandler(int syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch(syscall_num) {
        case SYSCALL_READ: // read
            return sys_read((int)arg1, (char*)arg2, (uint64_t)arg3);
        case SYSCALL_WRITE: // write  
            return sys_write((int)arg1, (const char*)arg2, (uint64_t)arg3);
        default:
            return -1;
    }
}


/*
 * ID 0
 */
int sys_read(int fd, char* buffer, uint64_t count) {
    switch(fd) {
        case STDIN: // stdin
            // Leer del buffer del teclado
            // Podrías implementar un buffer circular para las teclas
          //  return readFromKeyboard(buffer, count);
        default:
            return -1;
    }
}

/*
 * ID 1
 */
// En tu syscallHandler
int sys_write(int fd, const char* buffer, uint64_t count) {
    switch(fd) {
        case STDOUT: // stdout
        case STDERR: // stderr
            // Escribir directamente a pantalla usando tu videoDriver
	   
            for(uint64_t i = 0; i < count; i++) {
                putChar(buffer[i], 0xFF0000, 0x00FF00, 0 + (i * 8 * 5), 0, 5); // Asumiendo que tienes esta función
            }
            return count;
        default:
            return -1; // Error: fd no válido
    }
}
