#include <stdio.h>

int main(int argc, char const* argv[]) {
    int value = 0xffffffff;
    value = value & ~(1 << 5);  // ビット5を0に設定する
    value = value & ~(1 << 7);  // ビット7を0に設定する

    printf("%x\n", value);

    if ((value & ((1 << 5) | (1 << 7))) == 0) {
        printf("pass\n");
    } else {
        printf("fail. %x\n", value);
    }

    return 0;
}
