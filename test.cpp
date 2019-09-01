#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include "memorypool.h"
using namespace std;
//#define USEPOOL

int main(){
    MemoryPool pool(10); //1Mb
    clock_t start,end;
    int sum=0;
    vector<int> randnum(20005);
    for(int i=0;i<20000;i++){
        randnum[i]=rand()%128;
    }

#ifdef USEPOOL
    for(int i=0;i<80;i++){
        start=clock();
        for(int i=0;i<20000;i++){
            char *p=(char*)pool.allocate(randnum[i]);

            pool.deallocate(p,randnum[i]);
        }
        end=clock();
        sum+=(end-start);
    }
    cout<<"MemoryPool:20000次申请释放内存(0-128bytes)用时:"<<sum/80<<"毫秒"<<endl;
#else
    sum=0;
    for(int i=0;i<80;i++){
        start=clock();
        for(int i=0;i<20000;i++){
            char *p=(char*)malloc(randnum[i]);
            free(p);
        }
        end=clock();
        sum+=(end-start);
    }
        cout<<"原生api:20000次申请释放内存(0-128bytes)用时:"<<sum/80<<"毫秒"<<endl;

#endif

    return 0;
}

// g++ test.cpp defaultmalloc.h memorypool.h -o test
