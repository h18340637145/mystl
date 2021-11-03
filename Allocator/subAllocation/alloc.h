#pragma once

#include <string.h> //memcpy
#include <stdlib.h>//malloc  free
#include <new> //bad_alloc

namespace MiniSTL
{
    
    //一级配置器
    //直接调用malloc free    申请空间大于128bytes
    //一级配置器的allocate()   realloc()  都是在调用malloc()和realloc()不成功后，
    //改调用oom_malloc()  oom_realloc(),后面的内循环在不断的调用内存不足处理例程
    //期望在某次调用之后获得足够而圆满的任务
    //但如果内存处理例程并未被客户端设定，oom_malloc()  oom_realloc() 便不客气的调用bad_alloc()
    class __malloc_alloc{
        //alias declaration  别名声明
        using malloc_handler = void(*)();//指向返回类型为void的函数 的指针原型！！
        // 可用于声明这种指针  

    private:
        //以下函数指针用以处理内存不足的情况
        static void *oom_malloc(size_t);
        static void *oom_realloc(void* ,size_t);
        //void(*)() __malloc_alloc_oom_handler   返回类型为void的函数  用malloc_handler去指向并调用
        static malloc_handler __malloc_alloc_oom_handler;//它是个函数指针

    public:
        static void *allocate(size_t n)
        {
            void *result = malloc(n); // 直接调用malloc
            if (result == nullptr)
            {
                //申请失败，处理内存不足的情况,使用oom_malloc函数再次申请
                oom_malloc(n);
            }
            return result;
        }
        static void deallocate(void * p , size_t ){
            free(p);//释放p这块空间
        }

        static void* reallocate(void*p,size_t,size_t new_size){
            void * result = realloc(p,new_size); //直接重新分配空间
            if (result == nullptr){
                //如果内存空间不足
                //使用oom函数重新配置
                oom_realloc(p,new_size);//
            }
        }
    };

    //定义handler---内存不足时的处理
    typename __malloc_alloc::malloc_handler   //原型
        __malloc_alloc::__malloc_alloc_oom_handler = nullptr;//不处理，指向一个空函数

    //内存不足时的处理---malloc
    void * __malloc_alloc::oom_malloc(size_t n){
        malloc_handler new_alloc_handler;//指向返回类型为void的函数的指针   原型用于声明这种指针
        //通过malloc_handler声明的函数指针可以指向  自己实现的 处理内存不足的 函数
        void * result;
        for(;;){
            //不断尝试释放，配置
            new_alloc_handler = __malloc_alloc_oom_handler;//函数指针
            if (!new_alloc_handler){//不存在  抛异常   内存不足的处理失败，抛异常
                throw std::bad_alloc();// 相当于 new_alloc_handler == nullptr
            }
            (*new_alloc_handler)();//调用handler 试图释放内存
            result = malloc(n);//重新配置
            if(result != nullptr){
                return result;
            }
        }
    }

    //内存不足的处理----realloc
    void * __malloc_alloc::oom_realloc(void *p, size_t n){
        malloc_handler new_alloc_handler;
        void * result;
        for(;;){
            //不断的尝试配置
            new_alloc_handler = __malloc_alloc_oom_handler;
            if (!new_alloc_handler)
            {
                throw std::bad_alloc();
            }
            (*new_alloc_handler)();
            result = realloc(p,n);
            if(result){
                return result;
            }
        }
    }

    using malloc_alloc = __malloc_alloc;//一级配置器的名字重命名

    //freelist参数设定
    //区块上调边界，区块上限，freelist个数
    //enum用法
    enum __freelist_setting{
        __ALIGN = 8,//8bytes一跳
        __MAX_BYTES = 128,//最大128bytes
        __NFREELISTS = __MAX_BYTES / __ALIGN //128/8 个链表组
    };

    //二级配置器
    //8 - 128 byts的使用空间使用二级配置器直接配置
    //内设内存池和使用链表
    class __default_alloc{
    private:
        //将bytes上调至8的倍数
        static size_t ROUND_UP(size_t bytes){
            return (((bytes) + static_cast<size_t>(__ALIGN) - 1) &
            ~(static_cast<size_t>(__ALIGN) - 1));
            // x & ~x  相当于x*2？
        }

    private:
        //free_list节点
        //由于union特性，并需要占用额外的内存空间
        union obj{
            union obj * free_list_link;//指向下一个节点
            char client_data[1];    //指向资源
        };
    private:
        static obj * volatile free_list[__NFREELISTS]; //volatile 不稳定易变的 悄悄的改变
        //决定使用第几号节点，从1算起
        static size_t FREELIST_INDEX(size_t bytes){
            return (bytes + static_cast<size_t>(__ALIGN) -1) / 
                    static_cast<size_t>(__ALIGN) - 1;
        }
        //8  16  24  32  40  48  56
        //eg: 50bytes  57 / 8 -1 = 6  第六个链表  一个节点48bytes

        //传回一个大小为n的对象，并且可能加入大小为n的其他区块到free_list
        static void *refill(size_t n);
        //配置一大块空间，可容纳nobjs个大小为size的区块
        //如果不便配置，nobjs可能会降低
        static char *chunk_alloc(size_t size,int &nobjs);

        //chunk allocation state 块分配状态
        static char *start_free;  // n内存池起始位置，只在chunk_alloc()中变化
        static char *end_free;    //内存池结束位置，只在chunk_alloc()中变化
        static size_t heap_size;

    public:
        static void* allocate(size_t n);//分配n大小的空间
        static void deallocate(void* p,size_t n);//释放地址p   大小为n
        static void* reallocate(void * p, size_t old_sz,size_t new_sz);
        //释放p处大小为old的空间，同时再在此处申请newsize的空间
    };

    //初始化
    char * __default_alloc::start_free = nullptr;//内存池起始位置
    char * __default_alloc::end_free = nullptr;
    size_t __default_alloc::heap_size = 0;
    //freelist表初始为nullptr
    __default_alloc::obj * volatile __default_alloc::free_list[__NFREELISTS] = {
        nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,
        nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr 
    };

    //当free_list无可用区块时，重新填充空间
    //新空间取自内存池，默认获取20个节点（区块）
    //若内存池不足，则获取的将小于20
    void *__default_alloc::refill(size_t n){
        int nobjs = 20;//obj就是区块
        //尝试调用chunk_alloc , 注意nobjs以pass_by_reference传入
        //chunk指向20个区块(obj)空间
        char * chunk = chunk_alloc(n,nobjs);//分配区块？  obj是节点类型
        obj * volatile *my_free_list;
        obj* result;
        obj* current_obj,*next_obj;

        //若只获取了一个区块，则直接分配给调用者，不加入free_list
        if(1 == nobjs) return (chunk);//将区块返回
        my_free_list = free_list + FREELIST_INDEX(n);//n这么大字节的从哪个块开始，
        //my_free_list作为起始这么大块的起始

        //在chunk空间内建立free_list
        result = reinterpret_cast<obj*>(chunk);
        //引导free_list指向内存池分配空间
        //chunk指向的内存直接分给用户，free_list指向剩下(19或更少)的区块
        *my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n);//my_free_list指向分配之后（+n之后的位置，表示应该把这n个分配出去）
        for (int i = 0; ; i++){
            //无限循环？
            current_obj = next_obj;
            next_obj = 
                reinterpret_cast<obj*>(reinterpret_cast<char*>(next_obj) + n);
            if (nobjs - 1 == i){
                current_obj->free_list_link = nullptr;//当前list没有空间了
                break;
            }else{
                current_obj->free_list_link = next_obj;
            }
        }
        return result;//chunk直接返回给用户
    }

    //默认size为8的整数倍
    //分配块
    char* __default_alloc::chunk_alloc(size_t size,int &nobjs){
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;//内存池剩余空间
        if (bytes_left >= total_bytes){
            //容量满足需求
            result = start_free;//内存池的起始位置
            start_free += total_bytes;//start_free应该是分配之后内存池的起始位置
            return result;//将内存池原起始位置分配给用户，这就是用户想要的size个大小的nobjs个空间
        }else if (bytes_left > size){
            //没有nobjs个，但是目前
            //容量至少满足一个区块要求  尽可能的分配给用户  下面nobjs都分配走total_bytes
            nobjs = static_cast<int>(bytes_left / size);//内存池有nobjs个块的容量
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;//分配之后的起始位置
            return result;
        }else{
            //内存池一个区块都无法提供
            size_t bytes_to_get =  //获取新的空间  2倍大小 + 比自己大一个的节点
            2 * total_bytes + 
                ROUND_UP(
                    heap_size >> 4
                );//向head申请注入的内存，heap_size将随着配置次数增加而增加
            
            if (bytes_left > 0)
            {
                //内存池还要一部分内存，为了不浪费分配给free_list
                obj* volatile *my_free_list = 
                free_list + FREELIST_INDEX(bytes_left);
                reinterpret_cast<obj*>(start_free)->free_list_link = *my_free_list;
                *my_free_list = reinterpret_cast<obj*>(start_free);
            }
            //配置heap空间以补充内存池
            start_free = reinterpret_cast<char *>(malloc(bytes_to_get));
            if (!start_free){//malloc error  start_free == nullptr
                //heap 空间不足 malloc失败
                obj * volatile *my_free_list;
                obj * p;
                //在free_list 中检查是否有符合需求的区块 <=128
                for (size_t i = 0; i < static_cast<size_t>(__MAX_BYTES);
                    i += static_cast<size_t>(__ALIGN)){
                    my_free_list = free_list + FREELIST_INDEX(i);//定位到每一个区块
                    p = *my_free_list;//每个区块的开头
                    if(p != nullptr){
                        //存在足以分配的区块
                        *my_free_list = p->free_list_link;//抽离当前区块
                        start_free = reinterpret_cast<char *>(p);
                        end_free = start_free + i;
                        return (chunk_alloc(
                            size,
                            nobjs));//递归调用以修正nobjs，此时必然进入else if分支
                    }
                }
                end_free = nullptr;//到处都找不内存
                //调用第一级适配器观察其能否分配内存或抛出异常
                start_free = reinterpret_cast<char*>(malloc_alloc::allocate(bytes_to_get));
            }
            heap_size += bytes_to_get;//已占用的堆内存  问题：如果分配失败也要加上吗？
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size,nobjs);//调用自身以修正nojbs
        }
    }
    void* __default_alloc::allocate(size_t n){
        obj * volatile * my_free_list;//每次从内存取数据，抗拒优化
        obj * result;
        //如果n大于128 则采用一级适配器
        if (n > __MAX_BYTES){
            return (malloc_alloc::allocate(n));
        }
        //选择采用第几区块
        my_free_list = free_list + FREELIST_INDEX(n);//头地址+区块所在地址
        result = *my_free_list;//记录头
        if(result == nullptr){
            //未找到可用free_list  准备填充free_list
            void * r = refill(ROUND_UP(n));
            return r;
        }
        //调整freelist
        *my_free_list = result->free_list_link;//指向这一块的链表区
        return result;//返回头
    }

    void __default_alloc::deallocate(void* p,size_t n){
        //p 不可为nullptr
        if(n > static_cast<size_t>(__MAX_BYTES))
            malloc_alloc::deallocate(p,n);//第一级配置器回收
        else{
            //寻找对应的free_list
            obj *volatile *my_free_list = free_list + FREELIST_INDEX(n);
            obj *q = reinterpret_cast<obj*>(p);
            //调整free_list，回收区块
            //p是要回收的起始位置
            q->free_list_link = *my_free_list;//指向这个区块的开头了  相当于整个回收了
            *my_free_list = q;//起始位置p=q 只是类型不同 
        }  
    }
    void * __default_alloc::reallocate(void* p,size_t old_sz,size_t new_sz){
        void *result;
        size_t copy_sz;
        if(old_sz > static_cast<size_t>(__MAX_BYTES) && 
            new_sz > static_cast<size_t>(__MAX_BYTES))
            return realloc(p,new_sz);//超出128  使用第一级

        if(ROUND_UP(old_sz) == ROUND_UP(new_sz))//在同一个区块上
            return p;
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result,p , copy_sz);//将小的放置在结果中，确保覆盖 
        deallocate(p,old_sz);//回收旧的
        return result;
    }
} // namespace MiniSTL

