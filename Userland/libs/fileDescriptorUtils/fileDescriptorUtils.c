#include "./fileDescriptorUtils.h"
#include "../mystring/mystring.h"
#include <stdarg.h>

// borra todo el file descriptor
void flush(int fd)
{
    unsigned char buff[STD_BUFF_SIZE];
    read(fd, buff, STD_BUFF_SIZE);
}

/* UTILIDADES STDIN */
// getchar bloqueante porque read es bloqueante: espera hasta obtener 1 byte válido de STDIN.
unsigned char getchar()
{
    unsigned char c = 0;
    int count;
    do {
        count = read(STDIN, &c, 1);
    } while (count <= 0);
    return c;
}

int scanf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int assigned = 0;

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            char buffer[STD_BUFF_SIZE];
            int bufIndex = 0;
            unsigned char c;

            // Saltar marcadores de fin
            do {
                c = getchar();
            } while (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\b');
            // Leer token hasta separador
            while (c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != '\v' && c != '\b' && bufIndex < STD_BUFF_SIZE - 1) {
                buffer[bufIndex++] = c;
                c = getchar();
            }
            buffer[bufIndex] = '\0';

            switch (*format)
            {
            case 'd':
            {
                int *dest = va_arg(args, int *);
                *dest = strtoint(buffer);
                assigned++;
                break;
            }
            case 'u':
            {
                unsigned int *dest = va_arg(args, unsigned int *);
                *dest = strtouint(buffer);
                assigned++;
                break;
            }
            case 'X':
            case 'x':
            {
                unsigned int *dest = va_arg(args, unsigned int *);
                *dest = strtohex(buffer);
                assigned++;
                break;
            }
            case 'o':
            {
                unsigned int *dest = va_arg(args, unsigned int *);
                *dest = strtooct(buffer);
                assigned++;
                break;
            }
            case 'i':
            {
                int *dest = va_arg(args, int *);
                *dest = strtoint_complete(buffer);
                assigned++;
                break;
            }
            case 'c':
            {
                char *dest = va_arg(args, char *);
                *dest = buffer[0];
                assigned++;
                break;
            }
            case 's':
            {
                char *dest = va_arg(args, char *);
                int i = 0;
                while (buffer[i])
                {
                    dest[i] = buffer[i];
                    i++;
                }
                dest[i] = 0;
                assigned++;
                break;
            }
            default:
                break;
            }
        }
        else
        {
            format++;
        }

        format++;
    }

    va_end(args);
    return assigned;
}

/* UTILIDADES STDOUT */
void putchar(char c)
{ // %c
    char str[1] = {c};
    write(STDOUT, str, 1);
}
void puts(const char *str)
{ // %s
    write(STDOUT, str, strlen(str));
}
void putuint(uint64_t value)
{ // %u
    if (value == 0)
    {
        putchar('0');
        return;
    }

    char buffer[21]; // 64bits = 20 digitos (max) + \0
    int i = 0;

    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10); // 1234 => "4321"
        value /= 10;
    }

    for (int j = i - 1; j >= 0; j--)
    { // imprimir en orden correcto
        putchar(buffer[j]);
    }
}
void putint(int64_t value)
{ // %d %ld %lld
    if (value < 0)
    {
        putchar('-');
        value = -value;
    }
    putuint(value);
}
void putoct(uint64_t value)
{ // %o
    if (value == 0)
    {
        putchar('0');
        return;
    }

    char buffer[22]; // 64bits = 21 digitos en octal (max) + \0
    int i = 0;

    while (value > 0)
    {
        buffer[i++] = '0' + (value % 8); // 1234 => "4321"
        value /= 8;
    }

    for (int j = i - 1; j >= 0; j--)
    { // imprimir en orden correcto
        putchar(buffer[j]);
    }
}
void puthex(uint64_t value)
{ // %x %p
    if (value == 0)
    {
        putchar('0');
        return;
    }

    char hex_digits[16] = "0123456789abcdef";

    char buffer[16]; // 64bits = 16 digitos en hexa (max) + \0
    int i = 0;

    while (value > 0)
    {
        buffer[i++] = hex_digits[value % 16];
        value /= 16;
    }

    for (int j = i - 1; j >= 0; j--)
    {
        putchar(buffer[j]);
    }
}
void puthexupper(uint64_t value)
{ // %X %P
    if (value == 0)
    {
        putchar('0');
        return;
    }

    char hex_digits[16] = "0123456789ABCDEF";

    char buffer[16]; // 64bits = 16 digitos en hexa (max) + \0
    int i = 0;

    while (value > 0)
    {
        buffer[i++] = hex_digits[value % 16];
        value /= 16;
    }

    for (int j = i - 1; j >= 0; j--)
    {
        putchar(buffer[j]);
    }
}

static void write_char_fd(int fd, char c){ write(fd, &c, 1); }

uint64_t fprintf(int fd, const char *format, ...){
    if(!format) return 0;
    va_list args; va_start(args, format);
    uint64_t count_out = 0;
    while(*format){
        char c = *format++;
        if(c != '%'){ write_char_fd(fd,c); count_out++; continue; }
        c = *format++;
        if(c=='%'){ write_char_fd(fd,'%'); count_out++; continue; }
        int longFlag=0, longLongFlag=0, altFlag=0;
        // soporte basico de modificadores (# y l / ll)
        int parsing=1; while(parsing){
            switch(c){
                case '#': altFlag=1; c=*format++; break;
                case 'l': if(longFlag){ longLongFlag=1; } else longFlag=1; c=*format++; break;
                default: parsing=0; break;
            }
        }
        switch(c){
            case 'c': {
                char ch=(char)va_arg(args,int); write_char_fd(fd,ch); count_out++; break;
            }
            case 's': {
                const char *s=va_arg(args,const char*); if(!s) s="(null)"; size_t len=strlen(s); write(fd,s,len); count_out+=len; break; }
            case 'd': case 'u': case 'x': case 'X': case 'o': case 'p': case 'P': {
                unsigned long long val=0; int is_signed=(c=='d');
                if(is_signed){ long long sv = longLongFlag? va_arg(args,long long) : (longFlag? va_arg(args,long) : va_arg(args,int)); if(sv<0){ write_char_fd(fd,'-'); count_out++; val=(unsigned long long)(-sv); } else val=(unsigned long long)sv; }
                else { val = longLongFlag? va_arg(args,unsigned long long) : (longFlag? va_arg(args,unsigned long) : va_arg(args,unsigned int)); }
                char buf[64]; int idx=0; int base=10; const char *digits=(c=='X'||c=='P')?"0123456789ABCDEF":"0123456789abcdef"; if(c=='o') base=8; else if(c=='x'||c=='X'||c=='p'||c=='P') base=16;
                if(val==0){ buf[idx++]='0'; }
                else { while(val && idx < (int)sizeof(buf)){ buf[idx++] = digits[val % base]; val /= base; } }
                // alt flag
                if(altFlag){ if(base==16){ write(fd,(digits=="0123456789ABCDEF")?"0x":"0x",2); count_out+=2; } else if(base==8){ write_char_fd(fd,'0'); count_out++; } }
                while(idx--){ write_char_fd(fd, buf[idx]); count_out++; }
                break; }
            default: write_char_fd(fd,'%'); write_char_fd(fd,c); count_out+=2; break;
        }
    }
    va_end(args);
    return count_out;
}

uint64_t printf(const char *format, ...){
    va_list ap; va_start(ap, format);
    // Usar fprintf con buffer dinamico pasandole los argumentos manualmente no es trivial; reusar implementacion: construir lista de formato recorriendo dos veces.
    // Para simplicidad: delegar a vfprintf interno replicando logica (wrap):
    uint64_t out=0; const char *f=format; // reutilizamos fprintf pieza copiando logica parcial sin duplicar extraction
    // Implementacion simple: crear copia leyendo formato y usando va_list en una funcion auxiliar
    // Para evitar duplicar todo, iteramos igual que fprintf pero escribiendo en STDOUT
    while(*f){ char c=*f++; if(c!='%'){ write(STDOUT,&c,1); out++; continue; } c=*f++; if(c=='%'){ write(STDOUT,&c,1); out++; continue; }
        int longFlag=0,longLongFlag=0,altFlag=0; int parsing=1; while(parsing){ switch(c){ case '#': altFlag=1; c=*f++; break; case 'l': if(longFlag){ longLongFlag=1; } else longFlag=1; c=*f++; break; default: parsing=0; break; } }
        switch(c){
            case 'c': { char ch=(char)va_arg(ap,int); write(STDOUT,&ch,1); out++; break; }
            case 's': { const char *s=va_arg(ap,const char*); if(!s) s="(null)"; size_t len=strlen(s); write(STDOUT,s,len); out+=len; break; }
            case 'd': case 'u': case 'x': case 'X': case 'o': case 'p': case 'P': {
                unsigned long long val=0; int is_signed=(c=='d'); if(is_signed){ long long sv = longLongFlag? va_arg(ap,long long) : (longFlag? va_arg(ap,long) : va_arg(ap,int)); if(sv<0){ char m='-'; write(STDOUT,&m,1); out++; val=(unsigned long long)(-sv);} else val=(unsigned long long)sv; }
                else { val = longLongFlag? va_arg(ap,unsigned long long) : (longFlag? va_arg(ap,unsigned long) : va_arg(ap,unsigned int)); }
                char buf[64]; int idx=0; int base=10; const char *digits=(c=='X'||c=='P')?"0123456789ABCDEF":"0123456789abcdef"; if(c=='o') base=8; else if(c=='x'||c=='X'||c=='p'||c=='P') base=16; if(val==0){ buf[idx++]='0'; } else { while(val && idx<(int)sizeof(buf)){ buf[idx++]=digits[val%base]; val/=base; } }
                if(altFlag){ if(base==16){ write(STDOUT,"0x",2); out+=2; } else if(base==8){ char z='0'; write(STDOUT,&z,1); out++; } }
                while(idx--){ write(STDOUT,&buf[idx],1); out++; } break; }
            default: { char pct='%'; write(STDOUT,&pct,1); write(STDOUT,&c,1); out+=2; break; }
        }
    }
    va_end(ap); return out;
}
