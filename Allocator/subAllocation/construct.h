#pragma once

#include <new> // placement new 
#include "typeTraits.h"
//
//作用：对已分配的内存进行调用构造函数和析构函数
    // 调用构造函数使用p指针，和_T1类型的构造函数进行初始化
    //析构函数  判断_value_type类型是否有  无价值的析构函数   （pod类型都有无价值的 ctor）  
    // 对于无价值的 -- 不做析构函数调用处理
namespace MiniSTL{
    template<class T1,class T2>
    inline void construct(T1 *p,T2 value){
        new (p) T1(value);
    }

    template<class T>
    inline void destory(T *p){
        p->~T();
    }

    //设法利用traits批量析构对象
    template <class ForwardIterator>
    inline void destory(ForwardIterator beg,ForwardIterator end){
        using is_POD_type =  typename _type_traits<ForwardIterator>::is_POD_type;
        //pod类型的  不用析构
        _destory_aux(beg,end,is_POD_type());
    }

    //如果元素的value_type不存在non_trival destructor
    //一般的类类型  不是pod类型_false_type   需要析构
    template <class ForwardIterator>
    inline void _destory_aux(ForwardIterator beg,ForwardIterator end,
    _false_type){
        for(; beg != end ; ++beg){
            destory(&*beg);   //  *beg  迭代器指向的内容  &(*beg) 对迭代器指向的内容取地址
        }//毕竟迭代器不是真正的地址
    }

    //存在trival destructor  _true_type   是pod类型 --- 无价值构造---不需要析构---空实现
    //如果对象的析构函数无关痛痒   那么反复调用它是一种效率上的巨大浪费
    template<class ForwardIterator>
    inline void _destory_aux(ForwardIterator,ForwardIterator,_true_type){}

    //针对char* wchar_t* 的特化  字符串？ pod？
    inline void destory(char * , char * ){}
    inline void destory(wchar_t*, wchar_t *){}
}