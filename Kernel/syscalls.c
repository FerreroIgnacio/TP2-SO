#include <videoDriver.h>
#include <syscalls.h>
#include <keyboardDriver.h>

int syscallHandler(int syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    switch(syscall_num) {
        case SYSCALL_READ: // read
            return sys_read((int)arg1, (char*)arg2, (uint64_t)arg3);
        case SYSCALL_WRITE: // write  
            return sys_write((int)arg1, (const char*)arg2, (uint64_t)arg3);
	    case SYSCALL_ISKEYDOWN:
		    return sys_isKeyDown((int)arg1);
	    case SYSCALL_GET_BOOTTIME:
		    return sys_getBootTime();
        case SYSCALL_GET_TIME:
		    sys_getTime(arg1, arg2, arg3);
            return 1;
        case SYSCALL_GET_DATE:
		    sys_getDate(arg1, arg2, arg3);    
            return 1;
        case SYSCALL_GET_VIDEO_DATA:
            sys_get_video_data((uint16_t*) arg1, (uint16_t*) arg2,(uint16_t*) arg3, (uint16_t*) arg4);
            return 1; 
        case SYSCALL_PUT_PIXEL:
	        sys_put_pixel((uint32_t)arg1, (uint64_t)arg2, (uint64_t)arg3);
        	return 1;
        case SYSCALL_SET_FRAMEBUFFER:
            sys_set_framebuffer((uint8_t*)arg1);
            return 1;
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
 * ID 2
 */
int sys_isKeyDown(int scancode) {
	return isKeyPressed(scancode);
}

static uint64_t system_ticks = 0;  // Contador de ticks global

// Handler interno, hay que moverlo
// Handler del timer (IRQ0)
void timerTickHandler() {
    system_ticks++;  // Incrementa el contador en cada tick
	//drawInt((int)system_ticks, 0x00FF00, 0x000000, 0, 0, 3);
    // putText((int)system_ticks, 0xFFFFFF, 0x000000, 0, getHeight() - 8 * 3 - 8 * 3, 3);
}

/*
 * ID 3
 */
uint64_t sys_getBootTime() {
    return system_ticks * 10;  // 100 Hz → 10 ms por tick
}

/*
 * ID 9 
 */
void sys_get_video_data(uint16_t* width, uint16_t* height, uint16_t* bpp, uint16_t* pitch){
    if (width)
        * width = getWidth();
    if (height)
        * height = getHeight();
    if (bpp)
        * bpp = getBpp();
    if (pitch)
        * pitch = getPitch();
}

/*
 * ID 10 
 */
void sys_put_pixel(uint32_t hexColor, uint64_t x, uint64_t y){
	putPixel(hexColor,x,y);
}

/*
 * ID 11 
 */
void sys_set_framebuffer(uint8_t * fb){
    setFramebuffer(fb);
}