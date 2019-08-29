#ifndef DEFAULT_MALLOC
#define DEFAULT_MALLOC

#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;

class DefaultMalloc{
public:
    static void * allocate(size_t n);
    static void * deallocate(void *p,size_t n);
    static void * reallocate(void *p,size_t old_sz,size_t new_sz);
    static void (*set_malloc_handler(void (*f)()))();
private:
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *p,size_t n);
    static void (* __malloc_alloc_oom_handler)();
};

void * DefaultMalloc::allocate(size_t n){
    void *result=malloc(n);
    if(result==0) result=oom_malloc(n);
    return result;
}

void * DefaultMalloc::deallocate(void *p,size_t n){
    free(p);
}

void * DefaultMalloc::reallocate(void *p,size_t old_sz,size_t new_sz){
    void * result=realloc(p,new_sz);
    if(result==0) result=oom_realloc(p,new_sz);
    return result;
}

void (*DefaultMalloc::set_malloc_handler(void (*f)()))(){
    void (*old)()=__malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler=f;
    return old;
}

void * DefaultMalloc::oom_malloc(size_t n){
    void (*my_malloc_handler)();
    void *result;
    for(;;){
        my_malloc_handler=__malloc_alloc_oom_handler;
        if(my_malloc_handler==0) exit(-1);
        (*my_malloc_handler)();
        result=malloc(n);
        if(result) return result;
    }
}

void * DefaultMalloc::oom_realloc(void *p,size_t n){
    void (*my_malloc_handler)();
    void *result;
    for(;;){
        my_malloc_handler=__malloc_alloc_oom_handler;
        if(my_malloc_handler==0) exit(-1);
        (*my_malloc_handler)();
        result=realloc(p,n);
        if(result) return result;
    }
    return result;
}

#endif
