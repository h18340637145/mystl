#pragma once

#include "alloc.h"
#include "construct.h"

namespace MiniSTL{
    //默认二级配置器
    template <class T, class Alloc = __default_alloc>
    class simpleAlloc{
    public: 
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    public:
    //分配内存空间  释放内存空间
        static T* allocate();
        static T* allocate(size_t n);
        static void deallocate(T* ptr);
        static void deallocate(T* ptr,size_t n);

        static void construct(T* ptr);
        static void construct(T* ptr, const T& value);
        static void destory(T* ptr);
        static void destory(T* first, T* last);
    };

    //静态方法
    template<class T,class Alloc>
    //无参分配  本质是分配一块大小为当前类型的空间
    T* simpleAlloc<T,Alloc>::allocate(){
        return reinterpret_cast<T*>(Alloc::allocate(sizeof(T)));
        //二级分配器中分配一块T大小的空间
    }
    
    template<class T,class Alloc>
    //size n 分配
    T* simpleAlloc<T,Alloc>::allocate(size_t n){
        if(n == 0) {
            return 0;
        }
        return reinterpret_cast<T*>(Alloc::allocate(sizeof(T) * n));//分配当前类型*n块的空间
    }

    //deallocate释放
    template<class T, class Alloc>
    void simpleAlloc<T,Alloc>::deallocate(T* ptr){
        Alloc::deallocate(reinterpret_cast<void*>(ptr),sizeof(T));
        //释放ptr指向的 一个类型那么大的空间
    }

    template<class T,class Alloc>
    void simpleAlloc<T,Alloc>::deallocate(T* ptr,size_t n){
        //释放ptr指向的  n这么大的空间
        if(n == 0){
            return ;
        }
        Alloc::deallocate(reinterpret_cast<void*>(ptr),sizeof(T) * n);
        //释放从ptr指向的地方开始  n*类型那么大的空间
    }

    //构造
    template<class T, class Alloc>
    void simpleAlloc<T,Alloc>::construct(T* ptr){
        new (ptr) T();// 在ptr指向的位置  调用T的构造函数
    }

    template<class T,class Alloc>
    void simpleAlloc<T,Alloc>::construct(T* ptr,const T& value){
        new (ptr) T(value);
    }

    //析构
    template<class T,class Alloc>
    void simpleAlloc<T,Alloc>::destory(T *ptr){
        //析构ptr指向的 
        ptr->~T();
    }

    template<class T,class Alloc>
    //析构区间一次调用迭代器指向的对象的析构 first->~T()
    void simpleAlloc<T,Alloc>::destory(T* first,T * last){
        for(;first != last;++first){
            first->~T();
        }
    }
}