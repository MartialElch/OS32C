#include <types.h>
#include <lib.h>
#include <keyboard.h>
#include <terminal.h>
#include <system.h>

extern char keybuffer[];
extern int keybuffer_read_pos;
extern int keybuffer_write_pos;

void exit(__attribute__ ((unused)) int status){
    systemhalt();
    return;
}

int getchar(void) {
    int c;

    while (keybuffer_read_pos == keybuffer_write_pos) {
        __asm__ volatile (
            "hlt"
        );
    }

    c = keybuffer[keybuffer_read_pos++];

    if (keybuffer_read_pos == KEYBUFFER_MAX) {
        keybuffer_read_pos = 0;
    }

    return c;
}

char *itoa(int value, char *string, int radix) {
    char temp[32];
    int i = 0;
    int is_negative = 0;
    
    if (value == 0) {
        string[0] = '0';
        string[1] = '\0';
        return string;
    }
    
    if (value < 0 && radix == 10) {
        is_negative = 1;
        value = -value;
    }
    
    while (value != 0) {
        int rem = value % radix;
        temp[i++] = (rem > 9) ? (rem - 10) + 'A' : rem + '0';
        value = value / radix;
    }
    
    int j = 0;
    if (is_negative) {
        string[j++] = '-';
    }
    
    while (i > 0) {
        string[j++] = temp[--i];
    }
    string[j] = '\0';

    return string;
}

int printf(const char *format, ...) {
    char *s, buffer[32];
    const char *p = format;
    int i, n=0, val, width=0;
	char pad = ' ';

    /* get pointer to variable argument list */
    __builtin_va_list args;
    __builtin_va_start(args, format);

    while (*p != '\0') {
        if (*p == '%') {
            p++;

			if (*p == '0') {					/* optional pad with 0 */
				pad = '0';
				p++;
			}

			while ((*p >= '0') && (*p <= '9')) {/* optional field length*/
				width = width*10 + (*p - '0');
				p++;
			}

            switch (*p) {
                case 'd': {                     /* decimal integer */
                    val = __builtin_va_arg(args, int);
                    itoa(val, buffer, 10);
                    s = buffer;

                    /* pad to width */
                    for (i=strlen(buffer); i<width; i++) {
                        putchar(pad);
                        n++;
                    }

                    while (*s != '\0') {
                        putchar(*s);
                        s++;
                        n++;
                    }
                    break;
                }
                case 'x': {                     /* hexadecimal integer */
                    val = __builtin_va_arg(args, int);
                    itoa(val, buffer, 16);
                    s = buffer;

                    /* pad to width */
                    for (i=strlen(buffer); i<width; i++) {
                        putchar(pad);
                        n++;
                    }

                    while (*s != '\0') {
                        putchar(tolower(*s));
                        s++;
                        n++;
                    }
                    break;
                }
                default: {                      /* unknown */
                    buffer[0] = '%';
                    buffer[1] = *p;
                    buffer[2] = '\0';
                    s = buffer;
                    while (*s != '\0') {
                        putchar(*s);
                        s++;
                        n++;
                    }
                    break;
                }
            }
            p++;
        } else {
            /* regular character */
            putchar(*p);
            p++;
            n++;
        }
    }

    __builtin_va_end(args);

    return n;
}

int putchar(int c) {
    writeChar(c);

    return c;
}

int puts(const char *s) {
    while (*s != '\0') {
        putchar(*s);
        s++;
    }
    putchar('\n');

    return 1;
}

int strcmp(const char *s1, const char *s2) {
    char c;
    int rc = 0;
    int i=0;

    while((c = s1[i]) != '\0') {
        if (s2[i] == '\0') {
            rc = -1;
            break;
        } else if (s2[i] != c) {
            rc = -1;
            break;
        }
        i++;
    }

    if (s2[i] != '\0') {
        rc = 1;
    }

    return rc;
}

int strlen(const char* str) 
{
	int len = 0;
	while (str[len]) {
		len++;
	}

	return len;
}

int tolower(int c) {
	if ((c >= 'A') && (c <= 'Z')) {
		c = c - ('A' - 'a');
	}

	return c;
}
