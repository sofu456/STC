
#include <stdio.h>

#define i_tag i
#define i_cap 100
#define i_val int
#include <stc/cstack.h>

#define i_tag c
#define i_val char
#include <stc/cstack.h>

int main() {
    c_auto (cstack_i, stack)
    c_auto (cstack_c, chars)
    {
        c_forrange (int, i, 101)
            cstack_i_push(&stack, i*i);

        printf("%d\n", *cstack_i_top(&stack));

        c_forrange (int, i, 90)
            cstack_i_pop(&stack);

        c_foreach (i, cstack_i, stack)
            printf(" %d", *i.ref);
        puts("");
        printf("top: %d\n", *cstack_i_top(&stack));
    }
}
