# STC [ccommon](../include/stc/ccommon.h): RAII and iterator macros

The following macros are recommended to use, and they safe/have no side-effects.

### c_auto, c_autovar, c_autoscope, c_autodefer
General ***defer*** mechanics for resource acquisition. These macros allows you to specify the
freeing of the resources at the point where the acquisition takes place.
The **checkauto** utility described below, ensures that the `c_auto*` macros are used correctly.

| Usage                                  | Description                                          |
|:---------------------------------------|:-----------------------------------------------------|
| `c_auto (Type, var...)`                | `c_autovar (Type var=Type_init(), Type_drop(&var))`  |
| `c_autovar (Type var=init, end...)`    | Declare `var`. Defer `end...` to end of block        |
| `c_autoscope (init, end...)`           | Execute `init`. Defer `end...` to end of block       |
| `c_autodefer (end...)`                 | Defer `end...` to end of block                       |
| `c_breakauto` or `continue`            | Break out of a `c_auto*`-block/scope without memleak |

For multiple variables, use either multiple **c_autovar** in sequence, or declare variable outside
scope and use **c_autoscope**. Also, **c_auto** support up to 4 variables.
```c
c_autovar (uint8_t* buf = malloc(BUF_SIZE), free(buf))
c_autovar (FILE* f = fopen(fname, "rb"), fclose(f))
{
    int n = 0;
    if (f && buf) {
        n = fread(buf, 1, BUF_SIZE, f);
        doSomething(buf, n);
    }
}

c_autovar (cstr s = cstr_new("Hello"), cstr_drop(&s))
{
    cstr_append(&s, " world");
    printf("%s\n", cstr_str(&s));
}

c_auto (cstr, s1, s2)
{
    cstr_append(&s1, "Hello");
    cstr_append(&s1, " world");

    cstr_append(&s2, "Cool");
    cstr_append(&s2, " stuff");

    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}

MyData data;
c_autoscope (mydata_init(&data), mydata_destroy(&data))
{
    printf("%s\n", cstr_str(&mydata.name));
}

cstr s1 = cstr_new("Hello"), s2 = cstr_new("world");
c_autodefer (cstr_drop(&s1), cstr_drop(&s2))
{
    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}
```
**Example**: Load each line of a text file into a vector of strings:
```c
#include <errno.h>
#include <stc/cstr.h>

#define i_val_str
#include <stc/cvec.h>

// receiver should check errno variable
cvec_str readFile(const char* name)
{
    cvec_str vec = cvec_str_init(); // returned

    c_autovar (FILE* fp = fopen(name, "r"), fclose(fp))
    c_autovar (cstr line = cstr_null, cstr_drop(&line))
        while (cstr_getline(&line, fp))
            cvec_str_emplace_back(&vec, cstr_str(&line));
    return vec;
}

int main()
{
    c_autovar (cvec_str x = readFile(__FILE__), cvec_str_drop(&x))
        c_foreach (i, cvec_str, x)
            printf("%s\n", cstr_str(i.ref));
}
```
### The **checkauto** utility program (for RAII)
The **checkauto** program will check the source code for any misuses of the `c_auto*` macros which
may lead to resource leakages. The `c_auto*`- macros are implemented as one-time executed **for-loops**,
so any `return` or `break` appearing within such a block will lead to resource leaks, as it will disable
the cleanup/drop method to be called. A `break` may originally be intended to break a loop or switch
outside the `c_auto` scope.

NOTE: One must always make sure to unwind temporary allocated resources before a `return` in C. However, by using `c_auto*`-macros,
- it is much easier to automatically detect misplaced return/break between resource acquisition and destruction.
- it prevents forgetting to call the destructor at the end.

The **checkauto** utility will report any misusages. The following example shows how to correctly break/return
from a `c_auto` scope: 
```c
    int flag = 0;
    for (int i = 0; i<n; ++i) {
        c_auto (cstr, text)
        c_auto (List, list)
        {
            for (int j = 0; j<m; ++j) {
                List_push_back(&list, i*j);
                if (cond1())
                    break;  // OK: breaks current for-loop only
            }
            // WRONG:
            if (cond2())
                break;      // checkauto ERROR! break inside c_auto.

            if (cond3())
                return -1;  // checkauto ERROR! return inside c_auto
            
            // CORRECT:
            if (cond2()) {
                flag = 1;   // flag to break outer for-loop
                continue;   // cleanup and leave c_auto block
            }
            if (cond3()) {
                flag = -1;  // return -1
                continue;   // cleanup and leave c_auto block
            }
            ...
        }
        // do the return/break outside of c_auto
        if (flag < 0) return flag;
        else if (flag > 0) break;
        ...
    } // for
```

### c_forarray, c_forarray_p 
Iterate compound literal array elements
```c
// apply multiple push_backs
c_forarray (int, v, {1, 2, 3})
    cvec_i_push_back(&vec, *v);

// insert in existing map
c_forarray (cmap_ii_raw, v, { {4, 5}, {6, 7} })
    cmap_ii_insert(&map, v->first, v->second);

// even define an anonymous struct inside it (no commas allowed)
c_forarray (struct { int a; int b; }, v, { {1, 2}, {3, 4}, {5, 6} })
    printf("(%d %d) ", v->a, v->b);

// `c_forarray_p` is required for pointer type elements
c_forarray_p (const char*, v, {"Hello", "crazy", "world"})
    cstack_s_push(&stk, *v);
```

### c_foreach, c_forpair

| Usage                                    | Description                     |
|:-----------------------------------------|:--------------------------------|
| `c_foreach (it, ctype, container)`       | Iteratate all elements          |
| `c_foreach (it, ctype, it1, it2)`        | Iterate the range [it1, it2)    |
| `c_forpair (key, val, ctype, container)` | Iterate with structured binding |

```c
#define i_key int
#define i_val int
#define i_tag ii
#include <stc/csmap.h>
...
c_forarray (csmap_ii_value, v, { {23,1}, {3,2}, {7,3}, {5,4}, {12,5} })
    csmap_ii_insert(&map, v->first, v->second);

c_foreach (i, csmap_ii, map)
    printf(" %d", i.ref->first);
// out: 3 5 7 12 23

csmap_ii_iter it = csmap_ii_find(&map, 7);
c_foreach (i, csmap_ii, it, csmap_ii_end(&map))
    printf(" %d", i.ref->first);
// out: 7 12 23

c_forpair (id, count, csmap_ii, map)
    printf(" (%d %d)", *_.id, *_.count);
// out: (3 2) (5 4) (7 3) (12 5) (23 1)
```

### c_forrange
Declare an iterator and specify a range to iterate with a for loop. Like python's ***for i in range()*** loop:

| Usage                                         | Python equivalent                    |
|:----------------------------------------------|:-------------------------------------|
| `c_forrange (stop)`                           | `for _ in range(stop):`              |
| `c_forrange (i, stop) // IntType = size_t`    | `for i in range(stop):`              |
| `c_forrange (IntType, i, stop)`               | `for i in range(stop):`              |
| `c_forrange (IntType, i, start, stop)`        | `for i in range(start, stop):`       |
| `c_forrange (IntType, i, start, stop, step)`  | `for i in range(start, stop, step):` |

```c
c_forrange (5) printf("x");
// xxxxx
c_forrange (i, 5) printf(" %" PRIuMAX "", i);
// 0 1 2 3 4
c_forrange (int, i, -3, 3) printf(" %d", i);
// -3 -2 -1 0 1 2
c_forrange (int, i, 30, 0, -5) printf(" %d", i);
// 30 25 20 15 10 5
```

### c_find_if, c_find_in
Search linearily in containers using a predicate
```
// NOTE: it.ref is NULL if not found, not cvec_i_end(&vec).ref
// This makes it easier to test.
cvec_i_iter it;

// Search vec for first value > 2:
c_find_if(cvec_i, vec, it, *it.ref > 2);
if (it.ref) printf("%d\n", *it.ref);

// Search within a range:
c_find_in(csmap_str, it1, it2, it, cstr_contains(*it.ref, "hello"));
if (it.ref) cmap_str_erase_at(&map, it);
```

### c_new, c_alloc, c_alloc_n, c_drop, c_make

| Usage                          | Meaning                                 |
|:-------------------------------|:----------------------------------------|
| `c_new (type, value)`          | Move value to a new object on the heap  |
| `c_alloc (type)`               | `(type *) c_malloc(sizeof(type))`       |
| `c_alloc_n (type, N)`          | `(type *) c_malloc((N)*sizeof(type))`   |
| `c_drop (ctype, &c1, ..., &cN)` | `ctype_drop(&c1); ... ctype_drop(&cN)`    |
| `c_make(type){value...}`       | `(type){value...}` // c++ compatability |

```c
struct Pnt { double x, y, z; };
struct Pnt *pnt = c_new (struct Pnt, {1.2, 3.4, 5.6});
c_free(pnt);

int* array = c_alloc_n (int, 100);
c_free(array);

cstr a = cstr_new("Hello"), b = cstr_new("World");
c_drop(cstr, &a, &b);
```

### General predefined template parameter functions
```
int         c_default_cmp(const Type*, const Type*);
Type        c_default_clone(Type val);           // simple copy
Type        c_default_toraw(const Type* val);    // dereference val
void        c_default_drop(Type* val);           // does nothing

typedef     const char* crawstr;
int         crawstr_cmp(const crawstr* x, const crawstr* y);
bool        crawstr_eq(const crawstr* x, const crawstr* y);
uint64_t    crawstr_hash(const crawstr* x);
```

### c_malloc, c_calloc, c_realloc, c_free
Memory allocator for the entire library. Macros can be overloaded by the user.

### c_swap, c_arraylen
- **c_swap(type, x, y)**: Simple macro for swapping internals of two objects.
- **c_arraylen(array)**: Return number of elements in an array, e.g. `int array[] = {1, 2, 3, 4};`
