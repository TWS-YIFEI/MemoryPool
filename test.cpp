#include <iostream>
#include "memorypool.h"
using namespace std;

int main(){
    MemoryPool pool;
    for(int i=0;i<10;i++){
        char *p=(char*)pool.allocate(i*30);
        if(p==nullptr){
            cout<<"allocate failed!"<<endl;
            return 0;
        }else{
            cout<<"allocate sucess!  "<<i<<"  "<<endl;
        }
        pool.deallocate(p,i*30);
    }
    return 0;
}

