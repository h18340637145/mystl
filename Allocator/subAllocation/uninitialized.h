#pragma once
#include <cstring>
#include "construct.h"
#include "stl_algobase.h"
#include "stl_iterator.h"
#include "typeTraits.h"

namespace MiniSTL
{
    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first,
                                                InputIterator last,
                                                ForwardIterator result){
       is_PODType = typename _type_traits<value_type_t<InputIterator>>::is_ POD_type;
       return __uninitialized_copy_aux(first, last, result, isPODType());                                 
    }

    template <class InputIterator, class ForwardIterator>
    inline  ForwardIterator __uninitialized_copy_aux(InputIterator first,
                                                    InputIterator last,
                                                    ForwardIterator result,
                                                    _true_type){
        return MiniSTL::copy(first, last, result);
    }

    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first,
                                                    InputIterator last,
                                                    ForwardIterator result,
                                                    _false_type){
        ForwardIterator cur = result;
        for(;first != last; ++cur, ++first){
            construct(&*cur, *first); //cur指针出构造一个对象，其值为*first
        }                            
        return cur;
    }

    //针对char* ，wchar_t* 存在特化版本 memmove直接移动内存
    inline char * uninitialized_copy(const char* first, const char *last,
                                    char *result){
        memmove(result, first, last - first);
        return result + (last - first);//返回尾指针                                
    }

    inline wchar_t *uninitialized_copy(const wchar_t* first, const wchar_t* last,
                                    wchar_t* result){
        memmove(result, first, last - first);
        return result + (last - first);
    }


//填充所有
    template<class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                                    const T & value){
                                 //萃取出这个类/pod //萃取出迭代器指向的类型        //取这个类/pod的是否是pod
        using isPODType = typename _type_traits<value_type_t<ForwardIterator>>::is_POD_type ;
        __uninitialized_fill_aux(first, last, value, isPODType());
    }

    template<class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first,
                                        ForwardIterator last,const T& value,
                                        _true_type){
    //默认使用pod类型填充
        MiniSTL::fill(first, last, value);
    }

    template<class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first,
                                        ForwardIterator last,const T& value,
                                        _false_type){
        //非pod类型  类类型填充使用 构造器初始化
        ForwardIterator cur = first;
        for(; cur != last; cur++){
            construct(&*cur, value); //构造对象，其值为value填充
        }                                    
    }

    template<class ForwardIterator,class Size, class T>
    inline  ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n,
                                                const T& value){
        using isPODType = typename _type_traits<value_type_t<ForwardIterator>>::is_POD_type;
        return __uninitialized_fill_n_aux(first, n, value, isPODType());
    }

    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                    const T& value, _true_type){
        return MiniSTL::fill_n(first, n, value);
    }

    template<class ForwardIterator, class Size, class T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                const T& value,_false_type){
        //忽略异常处理
        //需要明确的是  一旦一个对象构造失败，则需要析构所有对象
        ForwardIterator cur = first;
        for(; n > 0;n--, cur++){
            construct(&*cur, value);
        }
        return cur;
    }

    //SGI_STL扩展
    // uninitialized_copy_copy   uninitialized_copy_fill,  uninitialized_fill_copy

    // __uninitialized_copy_copy
    // copies[first1,last1)   into [result,result + (last1 - first1)), and
    // copies [first2, last2) into [result, result + (last1 - first1) + (last2 - first2))  就是last2

    template<class InputIterator1, class InputIterator2, class ForwardIterator>
    inline ForwardIterator uninitialized_copy_copy(InputIterator1 first1,
                                                    InputIterator1 last1,
                                                    InputIterator2 first2,
                                                    InputIterator2 last2,
                                                    ForwardIterator result){
        ForwardIterator mid = MiniSTL::uninitialized_copy(first1, last1, result);
        try
        {
            return MiniSTL::uninitialized_copy(first2, last2, mid);
        }
        catch(std::exception &)
        {
            destory(result, mid); //开始到当前  全部销毁
            throw;
        }
    }

    // uninitialized_fill_copy
    // Fills [result, mid) with x, and copies [first, last) into [mid, mid+(last - first));?

    template <class ForwardIterator, class  T, class InputIterator>
    inline ForwardIterator uninitialized_fill_copy(ForwardIterator result,
                                                    ForwardIterator mid,
                                                    const T& val,
                                                    InputIterator first,
                                                    InputIterator last){
        //前一部分用一个固定值填充
        //后一部分使用一个区间的值填充
        //result---mid 使用val填充
        MiniSTL::uninitialized_fill(result, mid, val);
        
        try
        {
            //first --- last  拷贝到mid之后
            return MiniSTL::uninitialized_copy(first, last, mid);
        }
        catch(std::exception&)
        {
            //如果出错
            //result(开头) , mid(当前) 之间全销毁
            destory(result, mid);
            throw;
        }
    }
    // __uninitialized_copy_fill
    // Copies [first1, last1) into [first2, first2 + (last1 - first1)), and fills [first2 + (last1 - first1), last2) with x.
    //前一半用区间填充，后面用固定值填充
    template<class InputIterator, class ForwardIterator, class T>
    inline void uninitialized_copy_fill(InputIterator first1, InputIterator last1,
                                        ForwardIterator first2,
                                        ForwardIterator last2,
                                        const T& value){
        ForwardIterator mid2 = MiniSTL::uninitialized_copy(first1, last1, first2);
        try
        {
            MiniSTL::uninitialized_fill(mid2, last2, value);
        }
        catch(std::exception &)
        {
            destory(first1, mid2);
            throw;
        }
    }
} // namespace MiniSTL
