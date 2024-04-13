#include <stdio.h>

int main(int argc, char const* argv[]){
    int value = 0xffffffff;
    value = value & ~(1 << 5 | 1 << 7);
    printf("%x\n", value);
    if ((value & (1 << 5 | 1 << 7)) == 0){
        printf("pass\n");
    }
    else {
        printf("fail. %x\n", value);
    }
    return 0;
}
