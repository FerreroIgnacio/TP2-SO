#include "./mystring.h"
#include "../memory/memory.h"
size_t strlen(const char *str)
{
    int len = 0;
    while (*str++)
        len++;
    return len;
}
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}
int strncmp(const char *s1, const char *s2, uint64_t n)
{
    while (n-- && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    if (n == (uint64_t)-1)
        return 0;
    return (unsigned char)*s1 - (unsigned char)*s2;
}
char *strcpy(char *dest, const char *src)
{
    char *ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
}
char *strncpy(char *dest, const char *src, uint64_t n)
{
    char *ret = dest;
    while (n && (*dest++ = *src++))
        n--;
    while (n--)
        *dest++ = '\0';
    return ret;
}
char *strcat(char *dest, const char *src)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return ret;
}
char *strncat(char *dest, const char *src, uint64_t n)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while (n-- && (*src))
        *dest++ = *src++;
    *dest = '\0';
    return ret;
}

char *strchr(const char *s, int c)
{
    while (*s)
    {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }
    if (c == '\0')
        return (char *)s;
    return NULL;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (n--)
    {
        *d++ = *s++;
    }
    return dest;
}

int64_t strtoint(const char *str)
{
    int64_t sign = 1;
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }
    return sign * strtouint(str);
}
uint64_t strtouint(const char *str)
{
    int64_t toReturn = 0;
    while (*str >= '0' && *str <= '9')
    {
        toReturn = toReturn * 10 + (*str - '0');
        str++;
    }
    return toReturn;
}
uint64_t strtohex(const char *str)
{
    unsigned int toReturn = 0;
    while ((*str >= '0' && *str <= '9') ||
           (*str >= 'a' && *str <= 'f') ||
           (*str >= 'A' && *str <= 'F'))
    {
        toReturn *= 16;
        if (*str >= '0' && *str <= '9')
            toReturn += *str - '0';
        else if (*str >= 'a' && *str <= 'f')
            toReturn += *str - 'a' + 10;
        else
            toReturn += *str - 'A' + 10;
        str++;
    }
    return toReturn;
}
uint64_t strtooct(const char *str)
{
    unsigned int toReturn = 0;
    while (*str >= '0' && *str <= '7')
    {
        toReturn = toReturn * 8 + (*str - '0');
        str++;
    }
    return toReturn;
}
// genérico para uint en dec, hex y oct
uint64_t strtoint_complete(const char *str)
{
    if (str[0] == '0')
    {
        if (str[1] == 'x' || str[1] == 'X')
            return strtohex(str + 2);
        else
            return strtooct(str + 1);
    }
    return strtoint(str);
}

char *find_args(char *cmd)
{
    while (*cmd && *cmd != ' ')
        cmd++;
    if (*cmd == ' ')
    {
        *cmd++ = '\0';
        while (*cmd == ' ')
            cmd++;
    }
    return cmd;
}

char *strtok(char *str, const char *delim)
{
    static char *next;
    char *start;

    if (str != NULL)
        next = str;

    if (next == NULL || *next == '\0')
        return NULL;

    while (*next && strchr(delim, *next))
        next++;

    if (*next == '\0')
        return NULL;

    start = next;

    while (*next && !strchr(delim, *next))
        next++;

    if (*next)
    {
        *next = '\0';
        next++;
    }

    return start;
}

char *itoa_malloc(int value)
{
    int temp = value;
    int length = (value <= 0);

    while (temp != 0)
    {
        temp /= 10;
        length++;
    }

    char *str = malloc(length + 1);
    if (!str)
        return NULL;

    str[length] = '\0';

    int negative = 0;
    if (value < 0)
    {
        negative = 1;
        value = -value;
    }

    for (int i = length - 1; i >= negative; i--)
    {
        str[i] = (value % 10) + '0';
        value /= 10;
    }

    if (negative)
        str[0] = '-';

    return str;
}