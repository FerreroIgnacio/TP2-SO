#include <videoDriver.h>
#include <syscalls.h>
#include <keyboardDriver.h>

int syscallHandler(int syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch(syscall_num) {
        case SYSCALL_READ: // read
            return sys_read((int)arg1, (char*)arg2, (uint64_t)arg3);
        case SYSCALL_WRITE: // write  
            return sys_write((int)arg1, (const char*)arg2, (uint64_t)arg3);
	case SYSCALL_PUT_PIXEL:
	    sys_put_pixel((uint32_t)arg1, (uint64_t)arg2, (uint64_t)arg3);
        	return 1;
	case SYSCALL_ISKEYDOWN:
		return sys_isKeyDown((int)arg1);
	default:
            return -1;
    }
}


/*
 * ID 0
 */
int sys_read(int fd, char* buffer, uint64_t count) {
    switch (fd) {
        case STDIN: {
            uint64_t i = 0;
            while (i < count && stdin_has_data()) {  // Solo leer si hay datos
                buffer[i++] = consumeKey();
            }
            return i;  // Retorna cuántos caracteres se leyeron (puede ser 0)
        }
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
            // Escribir directamente a pantalla usando tu videoDrive
            for(uint64_t i = 0; i < count; i++) {
                putChar(buffer[i], 0xFF0000, 0x00FF00, 0 + (i * 8 * 5), 0, 5); // Asumiendo que tienes esta función
            }
            return count;
        default:
            return -1; // Error: fd no válido
    }
}
/*
 * ID 10 
 */
void sys_put_pixel(uint32_t hexColor, uint64_t x, uint64_t y){
	putPixel(hexColor,x,y);
}

/*
 * ID 2
 */
int sys_isKeyDown(int scancode) {
	return isKeyPressed(scancode);
}


