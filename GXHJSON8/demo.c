#include <stdio.h>
#include <stdlib.h>

#define kaddr(addr)\
({\
int tmp = addr;\
if (addr > 5) \
    tmp = 2;\
else\
    tmp = 3;\
(addr+tmp);\
})\

int main() {
    int addr = 4;
    int ans;

    ans = kaddr(addr);
    printf("%d.\n", ans);

    addr = 8;
    ans = kaddr(addr);
    printf("%d.\n", ans);

    // char*p = "hello";
    // printf("%c\n",*p++);
    char ch = 0x12;
    char buffer[7];
    sprintf(buffer,"\\u%04X",ch);
    printf("%s\n",buffer);

    char* p,*head;
    p = head = (char*)malloc(7);
    const char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    *p++ = '\\'; *p++ = 'u'; *p++ = '0'; *p++ = '0';
    *p++ = hex_digits[ch >> 4];
    *p++ = hex_digits[ch & 15];
    printf("%c %c\n",hex_digits[ch >> 4],hex_digits[ch & 15]);
    *p++ = '\0';
    printf("%s\n",head);
    return 0;
}