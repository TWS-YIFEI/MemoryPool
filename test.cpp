#include <iostream>
#include "memorypool.h"
using namespace std;

int main(){
    /*
    char *tt=(char*)malloc(0);
    if(tt==0){
        cout<<"failed"<<endl;
    }else{
        cout<<"sucess"<<endl;
    }
    tt=(char*)realloc(tt,9);
    cout<<(int*)tt<<endl;
    free(tt);
    */

    MemoryPool pool;
    for(int i=0;i<10;i++){
        char *p=(char*)pool.allocate(10);
        if(p==nullptr){
            cout<<"allocate failed!"<<endl;
            return 0;
        }else{
            cout<<(int*)p<<endl;
            cout<<"allocate sucess!  "<<i<<"  "<<endl;
        }
        pool.deallocate(p,10);
    }
    return 0;
}

// g++ test.cpp defaultmalloc.h memorypool.h -o test
