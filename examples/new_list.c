#include <stc/cstr.h>

forward_clist(clist_i32, int);
forward_clist(clist_pnt, struct Point);

struct MyStruct {
    clist_i32 intlst;
    clist_pnt pntlst;
} typedef MyStruct;

#define i_val int
#define i_opt c_is_fwd
#define i_tag i32
#define i_extern // define _clist_mergesort() 
#include <stc/clist.h>

struct Point { int x, y; } typedef Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_val Point
#define i_cmp point_cmp
#define i_opt c_is_fwd
#define i_tag pnt
#include <stc/clist.h>

#define i_val float
#include <stc/clist.h>

#define i_val_str
#include <stc/clist.h>


int main()
{
    c_auto (clist_i32, lst)
        clist_i32_push_back(&lst, 123);

    c_auto (clist_pnt, plst) {
        c_forarray (Point, v, {{42, 14}, {32, 94}, {62, 81}})
            clist_pnt_push_back(&plst, *v);

        clist_pnt_sort(&plst);

        c_foreach (i, clist_pnt, plst)
            printf(" (%d %d)", i.ref->x, i.ref->y);
        puts("");
    }

    c_auto (clist_float, flst) {
        c_forarray (float, v, {123.3f, 321.2f, -32.2f, 78.2f})
            clist_float_push_back(&flst, *v);
        
        c_foreach (i, clist_float, flst) printf(" %g", *i.ref);
    }

    c_auto (clist_str, slst)
        clist_str_emplace_back(&slst, "Hello, friend");
}
