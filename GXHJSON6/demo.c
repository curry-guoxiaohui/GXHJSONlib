#include <stdio.h>

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

    char*p = "hello";
    printf("%c\n",*p++);

    return 0;
}