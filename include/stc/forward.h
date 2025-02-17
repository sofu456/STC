/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef STC_FORWARD_H_INCLUDED
#define STC_FORWARD_H_INCLUDED

#include <stddef.h>

#define forward_carc(CX, VAL) _c_carc_types(CX, VAL)
#define forward_carr2(CX, VAL) _c_carr2_types(CX, VAL)
#define forward_carr3(CX, VAL) _c_carr3_types(CX, VAL)
#define forward_cbox(CX, VAL) _c_cbox_types(CX, VAL)
#define forward_cdeq(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_clist(CX, VAL) _c_clist_types(CX, VAL)
#define forward_cmap(CX, KEY, VAL) _c_chash_types(CX, KEY, VAL, uint32_t, c_true, c_false)
#define forward_cmap_huge(CX, KEY, VAL) _c_chash_types(CX, KEY, VAL, size_t, c_true, c_false)
#define forward_cset(CX, KEY) _c_chash_types(CX, cset, KEY, KEY, uint32_t, c_false, c_true)
#define forward_cset_huge(CX, KEY) _c_chash_types(CX, cset, KEY, KEY, size_t, c_false, c_true)
#define forward_csmap(CX, KEY, VAL) _c_aatree_types(CX, KEY, VAL, uint32_t, c_true, c_false)
#define forward_csset(CX, KEY) _c_aatree_types(CX, KEY, KEY, uint32_t, c_false, c_true)
#define forward_cstack(CX, VAL) _c_cstack_types(CX, VAL)
#define forward_cpque(CX, VAL) _c_cpque_types(CX, VAL)
#define forward_cqueue(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_cvec(CX, VAL) _c_cvec_types(CX, VAL)

typedef struct { char* data; size_t size, cap; } cstr_buf;
typedef char cstr_value;
#if defined STC_CSTR_V1
    typedef struct { char* str; } cstr;
#else
    typedef union {
        struct { char data[sizeof(cstr_buf) - 1]; unsigned char last; } sml;
        struct { char* data; size_t size, ncap; } lon;
    } cstr;
#endif

typedef struct { const char* str; size_t size; } csview;
typedef char csview_value;
typedef union { 
    const char *ref; 
    csview chr;
} csview_iter, cstr_iter;

#define c_true(...) __VA_ARGS__
#define c_false(...)

#define _c_carc_types(SELF, VAL) \
    typedef VAL SELF##_value; \
\
    typedef struct { \
        SELF##_value* get; \
        long* use_count; \
    } SELF

#define _c_carr2_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref; } SELF##_iter; \
    typedef struct { SELF##_value **data; size_t xdim, ydim; } SELF

#define _c_carr3_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref; } SELF##_iter; \
    typedef struct { SELF##_value ***data; size_t xdim, ydim, zdim; } SELF

#define _c_cbox_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { \
        SELF##_value* get; \
    } SELF

#define _c_cdeq_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct {SELF##_value *ref; } SELF##_iter; \
    typedef struct {SELF##_value *_base, *data;} SELF

#define _c_clist_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef struct { \
        SELF##_value *ref; \
        SELF##_node *const *_last, *prev; \
    } SELF##_iter; \
\
    typedef struct { \
        SELF##_node *last; \
    } SELF

#define _c_chash_types(SELF, KEY, VAL, SZ, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef SZ SELF##_size_t; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
\
    typedef struct { \
        SELF##_value *ref; \
        bool inserted, nomem_error; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref; \
        uint8_t* _hx; \
    } SELF##_iter; \
\
    typedef struct { \
        float max_load_factor; \
        SELF##_value* table; \
        uint8_t* _hashx; \
        SELF##_size_t size, bucket_count; \
    } SELF

#if defined STC_CSMAP_V1
#define _c_aatree_types(SELF, KEY, VAL, SZ, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef SZ SELF##_size_t; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
\
    typedef struct { \
        SELF##_value *ref; \
        bool inserted, nomem_error; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref; \
        int _top; \
        SELF##_node *_tn, *_st[36]; \
    } SELF##_iter; \
\
    typedef struct { \
        SELF##_node *root; \
        SELF##_size_t size; \
    } SELF
#else
#define _c_aatree_types(SELF, KEY, VAL, SZ, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef SZ SELF##_size_t; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
\
    typedef struct { \
        SELF##_value *ref; \
        bool inserted, nomem_error; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref; \
        SELF##_node *_d; \
        int _top; \
        SELF##_size_t _tn, _st[36]; \
    } SELF##_iter; \
\
    typedef struct { \
        SELF##_node *nodes; \
    } SELF
#endif
#define _c_cstack_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref; } SELF##_iter; \
    typedef struct SELF { \
        SELF##_value* data; \
        size_t size, capacity; \
    } SELF

#define _c_cstack_fixed(SELF, VAL, CAP) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref; } SELF##_iter; \
    typedef struct SELF { \
        SELF##_value data[CAP]; \
        size_t size; \
    } SELF

#define _c_cpque_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* data; \
        size_t size, capacity; \
    } SELF

#define _c_cvec_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref; } SELF##_iter; \
    typedef struct { SELF##_value *data; } SELF

#endif // STC_FORWARD_H_INCLUDED
