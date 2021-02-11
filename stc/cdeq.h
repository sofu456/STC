/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CDEQ__H__
#define CDEQ__H__

#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_cdeq(...) c_MACRO_OVERLOAD(using_cdeq, __VA_ARGS__)
#define using_cdeq_2(X, Value) \
                    using_cdeq_3(X, Value, c_default_compare)
#define using_cdeq_3(X, Value, valueCompare) \
                    using_cdeq_5(X, Value, valueCompare, c_default_del, c_default_clone)
#define using_cdeq_5(X, Value, valueCompare, valueDestroy, valueClone) \
                    using_cdeq_7(X, Value, valueCompare, valueDestroy, valueClone, c_default_to_raw, Value)
#define using_cdeq_str() \
                    using_cdeq_7(str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_to_raw, const char*)

#define typedefs_cdeq(X, Value, RawValue) \
    typedef Value cdeq_##X##_value_t; \
    typedef RawValue cdeq_##X##_rawvalue_t; \
    typedef struct { cdeq_##X##_value_t *ref; } cdeq_##X##_iter_t; \
    typedef struct { \
        cdeq_##X##_value_t *base, *data; \
    } cdeq_##X

struct cdeq_rep { size_t size, cap; void* base[]; };
#define _cdeq_rep(self) c_container_of((self)->base, struct cdeq_rep, base)

#define using_cdeq_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue) \
    typedefs_cdeq(X, Value, RawValue); \
\
    STC_API cdeq_##X \
    cdeq_##X##_init(void); \
    STC_INLINE bool \
    cdeq_##X##_empty(cdeq_##X deq) {return !_cdeq_rep(&deq)->size;} \
    STC_INLINE size_t \
    cdeq_##X##_size(cdeq_##X deq) {return _cdeq_rep(&deq)->size;} \
    STC_INLINE size_t \
    cdeq_##X##_capacity(cdeq_##X deq) {return _cdeq_rep(&deq)->cap;} \
    STC_INLINE Value \
    cdeq_##X##_value_from_raw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE cdeq_##X##_value_t \
    cdeq_##X##_value_clone(cdeq_##X##_value_t val) {return valueFromRaw(valueToRaw(&val));} \
    STC_API void \
    cdeq_##X##_clear(cdeq_##X* self); \
    STC_API void \
    cdeq_##X##_del(cdeq_##X* self); \
    STC_API void \
    _cdeq_##X##_expand(cdeq_##X* self, size_t n, bool at_front); \
    STC_API void \
    cdeq_##X##_resize(cdeq_##X* self, size_t size, Value fill_val); \
    STC_INLINE void \
    cdeq_##X##_reserve(cdeq_##X* self, size_t n) { \
        _cdeq_##X##_expand(self, (n - _cdeq_rep(self)->size)*2/3, false); \
    } \
    STC_INLINE void \
    cdeq_##X##_swap(cdeq_##X* a, cdeq_##X* b) {c_swap(cdeq_##X, *a, *b);} \
\
    STC_INLINE cdeq_##X \
    cdeq_##X##_with_size(size_t size, Value null_val) { \
        cdeq_##X x = cdeq_##X##_init(); \
        cdeq_##X##_resize(&x, size, null_val); \
        return x; \
    } \
    STC_INLINE cdeq_##X \
    cdeq_##X##_with_capacity(size_t size) { \
        cdeq_##X x = cdeq_##X##_init(); \
        _cdeq_##X##_expand(&x, size, false); \
        return x; \
    } \
    STC_API cdeq_##X \
    cdeq_##X##_clone(cdeq_##X vec); \
\
    STC_INLINE void \
    cdeq_##X##_shrink_to_fit(cdeq_##X *self) { \
        cdeq_##X x = cdeq_##X##_clone(*self); \
        cdeq_##X##_del(self); *self = x; \
    } \
\
    STC_API void \
    cdeq_##X##_push_n(cdeq_##X *self, const cdeq_##X##_rawvalue_t arr[], size_t n); \
    STC_API void \
    cdeq_##X##_push_back(cdeq_##X* self, Value value); \
    STC_INLINE void \
    cdeq_##X##_emplace_back(cdeq_##X* self, RawValue raw) { \
        cdeq_##X##_push_back(self, valueFromRaw(raw)); \
    } \
    STC_INLINE void \
    cdeq_##X##_pop_back(cdeq_##X* self) { \
        valueDestroy(&self->data[--_cdeq_rep(self)->size]); \
    } \
\
    STC_API void \
    cdeq_##X##_push_front(cdeq_##X* self, Value value); \
    STC_INLINE void \
    cdeq_##X##_emplace_front(cdeq_##X* self, RawValue raw) { \
        cdeq_##X##_push_front(self, valueFromRaw(raw)); \
    } \
    STC_INLINE void \
    cdeq_##X##_pop_front(cdeq_##X* self) { \
        valueDestroy(self->data++); \
        --_cdeq_rep(self)->size; \
    } \
\
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_insert_range_p(cdeq_##X* self, cdeq_##X##_value_t* pos, const cdeq_##X##_value_t* pfirst, const cdeq_##X##_value_t* pfinish); \
\
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_insert_range(cdeq_##X* self, cdeq_##X##_iter_t pos, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish) { \
        return cdeq_##X##_insert_range_p(self, pos.ref, first.ref, finish.ref); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_insert_at(cdeq_##X* self, cdeq_##X##_iter_t pos, Value value) { \
        return cdeq_##X##_insert_range_p(self, pos.ref, &value, &value + 1); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_insert(cdeq_##X* self, size_t idx, Value value) { \
        return cdeq_##X##_insert_range_p(self, self->data + idx, &value, &value + 1); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_emplace_at(cdeq_##X* self, cdeq_##X##_iter_t pos, RawValue raw) { \
        return cdeq_##X##_insert_at(self, pos, valueFromRaw(raw)); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_emplace(cdeq_##X* self, size_t idx, RawValue raw) { \
        return cdeq_##X##_insert(self, idx, valueFromRaw(raw)); \
    } \
\
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_erase_range_p(cdeq_##X* self, cdeq_##X##_value_t* first, cdeq_##X##_value_t* finish); \
\
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_erase_range(cdeq_##X* self, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish) { \
        return cdeq_##X##_erase_range_p(self, first.ref, finish.ref); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_erase_at(cdeq_##X* self, cdeq_##X##_iter_t pos) { \
        return cdeq_##X##_erase_range_p(self, pos.ref, pos.ref + 1); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_erase(cdeq_##X* self, size_t idx, size_t n) { \
        return cdeq_##X##_erase_range_p(self, self->data + idx, self->data + idx + n); \
    } \
\
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_find(const cdeq_##X* self, RawValue raw); \
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_find_in_range(const cdeq_##X* self, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish, RawValue raw); \
\
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_front(cdeq_##X* self) {return self->data;} \
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_back(cdeq_##X* self) {return self->data + _cdeq_rep(self)->size - 1;} \
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_at(cdeq_##X* self, size_t i) { \
        assert(i < _cdeq_rep(self)->size); \
        return self->data + i; \
    } \
\
    STC_API int \
    cdeq_##X##_value_compare(const cdeq_##X##_value_t* x, const cdeq_##X##_value_t* y); \
    STC_INLINE void \
    cdeq_##X##_sort_with(cdeq_##X* self, size_t ifirst, size_t ifinish, int(*cmp)(const cdeq_##X##_value_t*, const cdeq_##X##_value_t*)) { \
        qsort(self->data + ifirst, ifinish - ifirst, sizeof(Value), (_cdeq_cmp) cmp); \
    } \
    STC_INLINE void \
    cdeq_##X##_sort(cdeq_##X* self) { \
        cdeq_##X##_sort_with(self, 0, _cdeq_rep(self)->size, cdeq_##X##_value_compare); \
    } \
\
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_begin(const cdeq_##X* self) { \
        cdeq_##X##_iter_t it = {self->data}; return it; \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_end(const cdeq_##X* self) { \
        cdeq_##X##_iter_t it = {self->data + _cdeq_rep(self)->size}; return it; \
    } \
    STC_INLINE void \
    cdeq_##X##_next(cdeq_##X##_iter_t* it) {++it->ref;} \
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_itval(cdeq_##X##_iter_t it) {return it.ref;} \
    STC_INLINE size_t \
    cdeq_##X##_index(cdeq_##X deq, cdeq_##X##_iter_t it) {return it.ref - deq.data;} \
\
    _c_implement_cdeq_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue) \
    typedef cdeq_##X cdeq_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

static struct cdeq_rep _cdeq_inits = {0, 0};
#define              _cdeq_nfront(self) ((self)->data - (self)->base)
static inline double _minf(double x, double y) {return x < y ? x : y;}
static inline double _maxf(double x, double y) {return x > y ? x : y;}

#define _c_implement_cdeq_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF cdeq_##X \
    cdeq_##X##_init(void) { \
        cdeq_##X##_value_t *b = (cdeq_##X##_value_t *) _cdeq_inits.base; \
        cdeq_##X deq = {b, b}; return deq; \
    } \
\
    STC_DEF void \
    cdeq_##X##_push_n(cdeq_##X *self, const cdeq_##X##_rawvalue_t arr[], size_t n) { \
        if (!n) return; \
        _cdeq_##X##_expand(self, n, false); \
        cdeq_##X##_value_t* p = self->data + _cdeq_rep(self)->size; \
        for (size_t i=0; i < n; ++i) *p++ = valueFromRaw(arr[i]); \
        _cdeq_rep(self)->size += n; \
    } \
\
    STC_DEF void \
    cdeq_##X##_clear(cdeq_##X* self) { \
        struct cdeq_rep* rep = _cdeq_rep(self); if (rep->cap) { \
            for (cdeq_##X##_value_t *p = self->data, *q = p + rep->size; p != q; ++p) \
                valueDestroy(p); \
            rep->size = 0; \
        } \
    } \
    STC_DEF void \
    cdeq_##X##_del(cdeq_##X* self) { \
        cdeq_##X##_clear(self); \
        if (_cdeq_rep(self)->cap) \
            c_free(_cdeq_rep(self)); \
    } \
\
    STC_DEF void \
    _cdeq_##X##_expand(cdeq_##X* self, size_t n, bool at_front) { \
        struct cdeq_rep* rep = _cdeq_rep(self); \
        size_t len = rep->size, cap = rep->cap; \
        size_t nfront = self->data - self->base, nback = cap - (nfront + len); \
        if (at_front && nfront >= n || !at_front && nback >= n) \
            return; \
        if ((len + n)*1.3 > cap) { \
            cap = (len + n + 6)*1.8; \
            rep = (struct cdeq_rep*) c_realloc(rep->cap ? rep : NULL, \
                                               sizeof(struct cdeq_rep) + cap*sizeof(Value)); \
            rep->size = len, rep->cap = cap; \
            self->base = (cdeq_##X##_value_t *) rep->base; \
            self->data = self->base + nfront; \
            _cdeq_##X##_expand(self, n, at_front); \
            return; \
        } \
        size_t unused = cap - (len + n); \
        size_t pos = at_front ? _maxf(unused*0.5, (float) unused - nback) + n \
                              : _minf(unused*0.5, nfront); \
        self->data = (cdeq_##X##_value_t *) memmove(self->base + pos, self->data, len*sizeof(Value)); \
    } \
\
    STC_DEF void \
    cdeq_##X##_resize(cdeq_##X* self, size_t size, Value null_val) { \
        _cdeq_##X##_expand(self, size, false); \
        size_t i, n = _cdeq_rep(self)->size; \
        for (i=size; i<n; ++i) valueDestroy(self->data + i); \
        for (i=n; i<size; ++i) self->data[i] = null_val; \
        if (self->data) _cdeq_rep(self)->size = size; \
    } \
\
    STC_DEF void \
    cdeq_##X##_push_front(cdeq_##X* self, Value value) { \
        if (self->data == self->base) \
            _cdeq_##X##_expand(self, 1, true); \
        *--self->data = value; \
        ++_cdeq_rep(self)->size; \
    } \
    STC_DEF void \
    cdeq_##X##_push_back(cdeq_##X* self, Value value) { \
        if (_cdeq_nfront(self) + _cdeq_rep(self)->size == _cdeq_rep(self)->cap) \
            _cdeq_##X##_expand(self, 1, false); \
        self->data[_cdeq_rep(self)->size++] = value; \
    } \
\
    STC_DEF cdeq_##X \
    cdeq_##X##_clone(cdeq_##X vec) { \
        size_t len = _cdeq_rep(&vec)->size; \
        cdeq_##X out = cdeq_##X##_with_capacity(len); \
        cdeq_##X##_insert_range_p(&out, out.data, vec.data, vec.data + len); \
        return out; \
    } \
\
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_insert_range_p(cdeq_##X* self, cdeq_##X##_value_t* pos, \
                              const cdeq_##X##_value_t* first, const cdeq_##X##_value_t* finish) { \
        size_t n = finish - first, idx = pos - self->data, size = _cdeq_rep(self)->size; \
        bool at_front = (idx < size/2); \
        _cdeq_##X##_expand(self, n, at_front); \
        if (at_front) { \
            memmove(self->data - n, self->data, idx*sizeof(Value)); \
            pos = (self->data -= n) + idx; \
        } else { \
            pos = self->data + idx; \
            memmove(pos + n, pos, (size - idx)*sizeof(Value)); \
        } \
        cdeq_##X##_iter_t it = {pos}; \
        if (n) _cdeq_rep(self)->size += n; \
        while (first != finish) \
            *pos++ = valueFromRaw(valueToRaw(first++)); \
        return it; \
    } \
\
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_erase_range_p(cdeq_##X* self, cdeq_##X##_value_t* first, cdeq_##X##_value_t* finish) { \
        intptr_t len = finish - first; \
        if (len > 0) { \
            cdeq_##X##_value_t* p = first, *end = self->data + _cdeq_rep(self)->size; \
            while (p != finish) valueDestroy(p++); \
            if (first == self->data) self->data += len; \
            else memmove(first, finish, (end - finish) * sizeof(Value)); \
            _cdeq_rep(self)->size -= len; \
        } \
        cdeq_##X##_iter_t it = {first}; return it; \
    } \
\
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_find_in_range(const cdeq_##X* self, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish, RawValue raw) { \
        for (; first.ref != finish.ref; cdeq_##X##_next(&first)) { \
            RawValue r = valueToRaw(first.ref); \
            if (valueCompareRaw(&r, &raw) == 0) return first; \
        } \
        return cdeq_##X##_end(self); \
    } \
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_find(const cdeq_##X* self, RawValue raw) { \
        return cdeq_##X##_find_in_range(self, cdeq_##X##_begin(self), cdeq_##X##_end(self), raw); \
    } \
\
    STC_DEF int \
    cdeq_##X##_value_compare(const cdeq_##X##_value_t* x, const cdeq_##X##_value_t* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    }

#else
#define _c_implement_cdeq_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue)
#endif

#if defined(_WIN32) && defined(_DLL)
#define STC_EXTERN_IMPORT extern __declspec(dllimport)
#else
#define STC_EXTERN_IMPORT extern
#endif

typedef int(*_cdeq_cmp)(const void*, const void*);
STC_EXTERN_IMPORT void qsort(void *start, size_t nitems, size_t size, _cdeq_cmp cmp);

#endif
