# MemoryPool
当需要频繁的进行new和delete操作时，可能会导致产生很多内存碎片。
所以用一个内存池来对这些空间进行管理，可以有效地提高内存利用率。
另外也可以用内存池和placement new来一块使用。
在STL中也有一个内存池的实现，还是非常巧妙的。在此学习并模仿着写一个。

### test.cpp
```cpp
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include "memorypool.h"
using namespace std;
//#define USEPOOL

int main(){
    MemoryPool pool(10);
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
```

### 编译运行
```
g++ test.cpp defaultmalloc.h memorypool.h -o test
```

### 参考
* 注释版代码:https://taowusheng.cn
* 《STL源码剖析》
