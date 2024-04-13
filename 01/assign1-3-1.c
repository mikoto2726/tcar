#include <stdio.h>

int main(int argc, char const* argv[]){
    int a = 0x55, b = 0xaa;
    a = a | (0x01 << 3);
    if ((a & (0x01 << 3 )) != 0){
        printf("pass\n");
    }
    else {
        printf("fail\n");
    }
    return 0;
}