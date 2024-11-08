#include <stdio.h>
static int global_static_variable = 10;

int function(int argument_variable, int *pointer_variable, int argument_variable2) {
    static int local_static_variable = 20;

    int local_variable = 10;

    printf("global_static=%d, local_static=%d, local=%d, argument=%d, pointer=%d, argument2=%d\n",
           global_static_variable, local_static_variable, local_variable,
           argument_variable, *pointer_variable, argument_variable2);

    global_static_variable = argument_variable;
    local_static_variable = argument_variable;
    local_variable = argument_variable;
    argument_variable = argument_variable;
    argument_variable2 = argument_variable;
    *pointer_variable = argument_variable;
    return 0;
}

int main(int argc, char const* argv[])
{
    int main_local_variable1 = 1000;
    int main_local_variable2 = 1000;
    int main_local_variable3 = 1000;
    global_static_variable = -20;
    function(main_local_variable1, &main_local_variable2, main_local_variable3);
    // Q1. Answer the expected value of each variable after calling `function`.
    // global_static_variable=[1]
    // local_static_variable=[2]
    // local_variable=[3]
    // main_local_variable1=[4]
    // main_local_variable2=[5]
    // main_local_variable3=[6]

    main_local_variable3 = 2000;
    function(main_local_variable1, &main_local_variable2, main_local_variable3);
    // Q2. Answer the expected value of each variable after calling `function`.
    // global_static_variable=[7]
    // local_static_variable=[8]
    // local_variable=[9]
    // main_local_variable1=[10]
    // main_local_variable2=[11]
    // main_local_variable3=[12]
    return 0;
}