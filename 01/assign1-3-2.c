#include <stdio.h>

int main(int argc, char const* argv[]){
    int a = 0x55, b = 0xaa;
    b = b & ~(0x01 << 3);
    if ((b & (0x01 << 3 )) == 0){
        printf("pass\n");
    }
    else {
        printf("fail\n");
    }
    return 0;
}