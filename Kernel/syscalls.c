#include <videoDriver.h>
#include <syscalls.h>
#include <stdin.h>
#include <stdout.h>
#include <keyboardDriver.h>
#include <isrHandlers.h>

int syscallHandler(int syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    switch(syscall_num) {
        case SYSCALL_READ: // read
            return sys_read((int)arg1, (char*)arg2, (uint64_t)arg3);
        case SYSCALL_WRITE: // write  
            return sys_write((int)arg1, (const char*)arg2, (uint64_t)arg3);
	    case SYSCALL_ISKEYPRESSED:
		    return sys_isKeyPressed((int)arg1);
	    case SYSCALL_GET_BOOTTIME:
		    return sys_getBootTime();
        case SYSCALL_GET_TIME:
		    sys_getTime((uint8_t*)arg1, (uint8_t*)arg2, (uint8_t*)arg3);
            return 1;
        case SYSCALL_GET_DATE:
		    sys_getDate((uint8_t*)arg1, (uint8_t*)arg2, (uint8_t*)arg3);    
            return 1;
        case SYSCALL_GET_REGISTERS:
            sys_getRegisters((uint64_t*) arg1);
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
        case SYSCALL_SET_FRAMEBUFFER_REGION:
            sys_set_framebuffer_region((uint32_t)arg1, (uint32_t)arg2, (uint32_t)arg3, (uint32_t)arg4, (uint8_t*)arg5, (uint32_t)arg6);
            return 1;
        case SYSCALL_PLAY_SOUND:
            sys_playSound((uint16_t)arg1);
            return 1;
        case SYSCALL_STOP_SOUND:
            sys_stopSound();
            return 1;
        default:
            return -1;
    }
}


/*
 * ID 0
 */
int sys_read(int fd, char* buffer, uint64_t count) {
    if (buffer == 0 || count == 0)
        return 0;
    switch (fd) {
        case STDIN: {
            uint64_t i = 0;
            while (i < count && stdin_has_data()) {  // Solo leer si hay datos
                buffer[i++] = consumeKeyStdin();
            }
            return i;  // Retorna cuántos caracteres se leyeron (puede ser 0)
        }
        case STDOUT: {
            uint64_t i = 0;
            while (i < count && stdout_has_data()) {  // Solo leer si hay datos
                buffer[i++] = consumeKeyStdout();
            }
            buffer[i]=0;
            return i;  // Retorna cuántos caracteres se leyeron (puede ser 0)
        }

        default:
            return -1;
    }
}

/*
 * ID 1
 */
int sys_write(int fd, const char* buffer, uint64_t count) {
    if (buffer == 0 || count == 0)
        return 0;
    switch(fd) {
        case STDOUT: // stdout
            for (uint64_t i = 0; i < count; i++){
                char c = buffer[i];
                if ((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t' || c == '\b'){
                    queueKeyStdout(c);
                }
            }
            return count;

        case STDERR: // stderr
            // TODO: ver donde mandamos esto
            for(uint64_t i = 0; i < count; i++) {
                putChar(buffer[i], 0xFF0000, 0x00FF00, 0 + (i * 8 * 5), 0, 5);
            }
            return count;
        default:
            return -1; // Error: fd no válido
    }
}

/*
 * ID 2
 */
int sys_isKeyPressed(uint16_t makecode) {
	return isKeyPressed(makecode);
}


/*
 * ID 3
 */
uint64_t sys_getBootTime() {
    return getSysTicks() * 55;
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
/*
* ID 12
*/
void sys_set_framebuffer_region(uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, uint8_t* bmp, uint32_t maskColor){
   setFrameBufferRegion(topLeftX, topLeftY, width, height, bmp, maskColor);
}
