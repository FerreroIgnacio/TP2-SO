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
unsigned char getchar()
{
    unsigned char c = 0;
    int count;

    // versión bloqueante
    /*
    do{
        count = read(STDIN, &c, 1);
    }while (!count);
    */

    // versión no bloqueante
    count = read(STDIN, &c, 1);
    if (count == 0)
        return 0; // si no hay nada para leer, retorna 0
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
            do
            {
                c = getchar();
            } while (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\b');

            // Leer token hasta encontrar separador
            // el código es bloqueante, hasta no encontrar un caracter de fin no sigue ejecutando.
            while (c == 0 || (c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != '\v' && c != '\b' && bufIndex < STD_BUFF_SIZE - 1))
            {
                if (c != 0)
                {
                    buffer[bufIndex++] = c;
                }
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

uint64_t printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char c;
    uint64_t len = 0;
    while (*format)
    {
        c = *format++;
        len++;
        switch (c)
        {
        case '%':
            c = *format++;
            switch (c)
            {
            case '%':
                putchar('%');
                break;
            case 'c':
                char ch = (char)va_arg(args, int);
                putchar(ch);
                break;
            case 's':
                puts(va_arg(args, char *));
                break;
            case 'l':
                c = *format++;
                if (c == 'd')
                {
                    putint(va_arg(args, int64_t));
                    break;
                }
                if (c == 'l')
                {
                    c = *format++;
                    if (c == 'd')
                    {
                        putint(va_arg(args, int64_t));
                        break;
                    }
                    format--;
                }
                format--;
                break;
            case 'd':
                putint(va_arg(args, int64_t));
                break;
            case 'u':
                putuint(va_arg(args, uint64_t));
                break;
            case '#':
                c = *format++;
                if (c == 'x' || c == 'p')
                {
                    puts("0x");
                    puthex(va_arg(args, uint64_t));
                    break;
                }
                if (c == 'X' || c == 'P')
                {
                    puts("0x");
                    puthexupper(va_arg(args, uint64_t));
                    break;
                }
                if (c == 'o')
                {
                    putchar('0');
                    putoct(va_arg(args, uint64_t));
                    break;
                }
                format--;
                break;
            case 'p':
            case 'x':
                puthex(va_arg(args, uint64_t));
                break;
            case 'P':
            case 'X':
                puthexupper(va_arg(args, uint64_t));
                break;
            case 'o':
                putoct(va_arg(args, uint64_t));
                break;

                // Agregar soporte para float / double
                /*
                case '.':
                    c = *format++;
                    if(c < '0' || c > '9'){
                        format--;
                        break;
                    }
                    int precision = c - '0';
                    c = *format++;
                    if (c != 'f'){
                        format-=2;
                        break;
                    }
                    putdouble(va_arg(args,double),precision);
                    break;
                case 'f':
                    putdouble(va_arg(args,double),6);
                    break;
                default:
                    break;
                */
            }
            break;
        default:
            putchar(c);
            break;
        }
    }
    return len;
}
