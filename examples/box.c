/* cbox: heap allocated boxed type */
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person Person_new(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}

uint64_t Person_hash(const Person* a) {
    return cstr_hash(&a->name) ^ cstr_hash(&a->last);
}

int Person_cmp(const Person* a, const Person* b) {
    int c = cstr_cmp(&a->name, &b->name);
    return c ? c : cstr_cmp(&a->last, &b->last);
}

Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}

void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    c_drop(cstr, &p->name, &p->last);
}

#define i_type PBox
#define i_val_bind Person // binds Person_cmp, ...
#include <stc/cbox.h>

#define i_type Persons
#define i_val_arcbox PBox // informs that PBox is a smart pointer.
#include <stc/cvec.h>

int main()
{
    c_auto (Persons, vec)
    c_auto (PBox, p, q)
    {
        p = PBox_make(Person_new("Laura", "Palmer"));
        q = PBox_clone(p);
        cstr_assign(&q.get->name, "Leland");

        printf("orig: %s %s\n", cstr_str(&p.get->name), cstr_str(&p.get->last));
        printf("copy: %s %s\n", cstr_str(&q.get->name), cstr_str(&q.get->last));

        Persons_push(&vec, PBox_make(Person_new("Dale", "Cooper")));
        Persons_push(&vec, PBox_make(Person_new("Audrey", "Home")));

        // NB! Clone p and q to the vector using emplace_back()
        Persons_push(&vec, PBox_clone(p));
        Persons_push(&vec, PBox_clone(q));

        c_foreach (i, Persons, vec)
            printf("%s %s\n", cstr_str(&i.ref->get->name), cstr_str(&i.ref->get->last));
        puts("");

        // Look-up Audrey! Create a temporary Person for lookup.
        c_autovar (Person a = Person_new("Audrey", "Home"), Person_drop(&a)) {
            const PBox *v = Persons_get(&vec, a); // lookup
            if (v) printf("found: %s %s\n", cstr_str(&v->get->name), cstr_str(&v->get->last));
        }
        puts("");

        // Alternative to use cbox (when not placed in container).
        Person *she = c_new(Person, Person_new("Shelly", "Johnson"));
        printf("%s %s\n", cstr_str(&she->name), cstr_str(&she->last));
        c_delete(Person, she); // drop and free
        puts("");
    }
}
