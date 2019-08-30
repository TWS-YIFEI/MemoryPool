#ifndef MEMORY_POOL
#define MEMORY_POOL
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "defaultmalloc.h"
using namespace std;

enum{__ALIGN=8};
enum{__MAX_BYTES=128};
enum{__NUM_FREELISTS=16};

class MemoryPool{
public:
    MemoryPool();
    MemoryPool(size_t n);
    static void * allocate(size_t n);
    static void * deallocate(void *p,size_t n);
    static void * reallocate(void *p,size_t old_sz,size_t new_sz);

private:
    union obj{
        union obj *free_list_link;
        char data[1];
    };
    static size_t ROUND_UP(size_t bytes){
        return ( (bytes+__ALIGN-1) & ~(__ALIGN-1) );
    }
    static size_t FREELSIT_INDEX(size_t bytes){
        return ( (bytes+__ALIGN-1)/__ALIGN -1 );
    }

    static void *refill(size_t n);
    static char *chunk_alloc(size_t size,int &objs);//此处要用引用

    static obj * volatile free_list[__NUM_FREELISTS];
    static char *start_free,*end_free;
    static size_t heap_size;
};

MemoryPool::MemoryPool(){
    for(int i=0;i<16;i++){
        free_list[i]=nullptr;
    }
    start_free=0;
    end_free=0;
    heap_size=0;
}

MemoryPool::MemoryPool(size_t n){
    start_free=malloc(n);
    if(start_free==0){
        start_free=DefaultMalloc::oom_malloc(n);
    }
    end_free=start_free+n;
    heap_size=n;
}

void * MemoryPool::allocate(size_t n){
    obj * volatile * my_free_list;
    obj * result;
    if(n > (size_t)__MAX_BYTES){
        return (DefaultMalloc::allocate(n));
    }
    my_free_list=free_list+FREELSIT_INDEX(n);
    result=*my_free_list;
    if(result==0){
        void *r=refill(ROUND_UP(n));
        return r;
    }
    *my_free_list=result->free_list_link;
    return result;
}

void * MemoryPool::deallocate(void *p,size_t n){

}

void * MemoryPool::reallocate(void *p,size_t old_sz,size_t new_sz){

}


#endif
