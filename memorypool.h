#ifndef MEMORY_POOL
#define MEMORY_POOL
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "defaultmalloc.h"
using namespace std;

enum{__ALIGN=8};
enum{__MAX_BYTES=128};
enum{__NUM_FREELISTS=16};

class MemoryPool{
public:
    MemoryPool(){};
    MemoryPool(size_t n);
    static void * allocate(size_t n);
    static void deallocate(void *p,size_t n);
    static void * reallocate(void *p,size_t old_sz,size_t new_sz);

private:
    union obj{
        union obj *free_list_link;
        char data[1];
    };
    static size_t ROUND_UP(size_t bytes){
        return ( (bytes+__ALIGN-1) & ~(__ALIGN-1) );
    }
    static size_t FREELIST_INDEX(size_t bytes){
        return ( (bytes+__ALIGN-1)/__ALIGN -1 );
    }

    static void *refill(size_t n);
    static char *chunk_alloc(size_t size,int &objs);//此处要用引用

    static obj * volatile free_list[__NUM_FREELISTS];
    static char *start_free,*end_free;
    static size_t heap_size; //内存池申请到的总空间大小，包括被free_list串起来的。
};

MemoryPool::obj * volatile MemoryPool::free_list[__NUM_FREELISTS]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char* MemoryPool::start_free=0;
char* MemoryPool::end_free=0;
size_t MemoryPool::heap_size=0;

/*
MemoryPool::initMemoryPool(){
    for(int i=0;i<16;i++){
        free_list[i]=nullptr;
    }
    start_free=0;
    end_free=0;
    heap_size=0;
}
*/

MemoryPool::MemoryPool(size_t n){
    start_free=(char*)DefaultMalloc::allocate(n);
    end_free=start_free+n;
    heap_size=n;
}

void * MemoryPool::allocate(size_t n){
    obj * volatile * my_free_list;
    obj * result;
    if(n > (size_t)__MAX_BYTES || n<=0){
        return (DefaultMalloc::allocate(n));
    }
    my_free_list=free_list+FREELIST_INDEX(n);
    result=*my_free_list;
    if(result==0){
        void *r=refill(ROUND_UP(n));
        return r;
    }
    *my_free_list=result->free_list_link;
    return result;
}

void MemoryPool::deallocate(void *p,size_t n){
    obj * volatile * my_free_list;
    obj *q=(obj*)p;
    if(n > (size_t)__MAX_BYTES || n<=0){
        DefaultMalloc::deallocate(p,n);
        return ;
    }
    my_free_list=free_list+FREELIST_INDEX(n);
    q->free_list_link=*my_free_list;
    *my_free_list=q;
}

void * MemoryPool::reallocate(void *p,size_t old_sz,size_t new_sz){
    void * result;
    size_t copy_size;
    if(old_sz > (size_t)__MAX_BYTES || new_sz<=0 || old_sz<=0){
        return (DefaultMalloc::reallocate(p,old_sz,new_sz));
    }
    if(ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;
    result=allocate(new_sz);
    copy_size=new_sz>old_sz ? old_sz : new_sz;
    memcpy(result,p,copy_size);
    deallocate(p,old_sz);
    return result;
}

void * MemoryPool::refill(size_t n){
    int nobjs=20;
    char * chunk=chunk_alloc(n,nobjs);
    obj * volatile * my_free_list;
    obj *result,*current_obj,*next_obj;
    if(nobjs==1) return chunk;
    my_free_list=free_list+FREELIST_INDEX(n);
    result=(obj*)chunk;
    *my_free_list=next_obj=(obj*)(chunk+n);
    for(int i=1;;i++){
        current_obj=next_obj;
        next_obj=(obj*)((char*)current_obj+n);
        if((i+1)==nobjs){
            current_obj->free_list_link=0;
            break;
        }else{
            current_obj->free_list_link=next_obj;
        }
    }
    return result;
}
char * MemoryPool::chunk_alloc(size_t size,int &objs){
    char *result;
    size_t total_bytes=size*objs;
    size_t bytes_left=end_free-start_free;
    if(bytes_left>=total_bytes){
        result=start_free;
        start_free+=total_bytes;
        return result;
    }else if(bytes_left>=size){
        objs=bytes_left/size;
        total_bytes=size*objs;
        result=start_free;
        start_free+=total_bytes;
        return result;
    }else{
        size_t bytes_to_get=total_bytes*2+ROUND_UP(heap_size>>4);
        if(bytes_left>0){
            obj * volatile * my_free_list=free_list+FREELIST_INDEX(bytes_left);
            ((obj*)start_free)->free_list_link=*my_free_list;
            *my_free_list=(obj*)start_free;
        }
        start_free=(char*)malloc(bytes_to_get);
        if(0==start_free){
            obj *volatile * my_free_list,*p;
            for(int i=size;i<=__MAX_BYTES;i+=__ALIGN){
                my_free_list=free_list+FREELIST_INDEX(i);
                p=*my_free_list;
                if(p!=0){
                    *my_free_list=p->free_list_link;
                    start_free=(char*)p;
                    end_free=start_free+i;
                    return chunk_alloc(size,objs);
                }
            }
            end_free=0;
            start_free=(char*)DefaultMalloc::allocate(bytes_to_get);
        }
        heap_size+=bytes_to_get;
        end_free=start_free+bytes_to_get;
        return chunk_alloc(size,objs);
    }
}

#endif
