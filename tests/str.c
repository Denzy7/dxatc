#include <dxatc-utils/str.h>

#include <stdio.h>

int main()
{
    char small[4];
    size_t off;
    dxAtcStrApp(small, sizeof(small), &off, "too big to fit");
    dxAtcStrApp(small, sizeof(small), &off, "even when we append");
    printf("%s\n", small);
}
