#pragma once

#include <string.h> //memcmp
#include <cstddef>
#include "stl_function.h"
#include "stl_iterator.h"
#include "typeTraits.h"


namespace MiniSTL
{
    //不能保证长度相等
    template<class InputIterator1 , class InputIterator2>
    inline bool equal(InputIterator1 first1,InputIterator1 last1,
                        InputIterator2 first2){
        for(;first1 != last1;++first1,++first2){//第一个容器的遍历
            if(*first1 != *first2){
                return false;
            }
        }
        //仅仅是在第一个容器范围内比较，如果第二容器比第一个容器大，则比较不能保证
        return true;
    }
    
    //接受自定义比较器
    template<class InputIterator1, class InputItrator2,class BinaryPredicate>
    inline bool equal(InputIterator1 first1,InputIterator1 last1,
                        InputItrator2 first2,BinaryPredicate binary_pred){
        for(;first1 != last1;first1++,first2++){
            if(!(*first1,*first2)){
                return false;
            }
        }
        return true;
    }

    //填充，将[)区间使用value填充
    template<class ForwardIterator,class T>
    void fill(ForwardIterator first, ForwardIterator last, const T& value){
        for(; first != last ; ++first){
            *first = value;
        }
    }

    //显然，本算法执行覆写操作，因此通常配合inserter完成
    //从first开始  n个数据都赋值为value，返回结尾处
    template<class OutputIterator, class Size,class T>
    OutputIterator fill_n(OutputIterator first,Size n,const T &value){
        for(;n > 0;--n,++first){
            *first = value;
        }
        return first;
    }

    template<class ForwardIterator1,class ForwardIterator2>
    inline void iter_swap(ForwardIterator1 a,ForwardIterator2 b){
        return __iter_swap(a,b,value_type_t<ForwardIterator1>());
    }

    //  直到迭代器指向对象的类型，才能构造对象，因此本初使用了value_type
    template<class ForwardIterator1,class ForwardIterator2,class T>
    inline void iter_swap(ForwardIterator1 a,ForwardIterator2 b,T){
        T temp = *a;
        *a = *b;
        *b = temp;
    }

    template<class InputIterator1,class InputIterator2>
    bool lexicographical_compare(InputIterator1 first1,InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2){
        for(;first1 != last1 && first2 != last2;++first1,++first2){
            if(*first1 < *first2){
                return true;
            }else if(*first1 > *first2){
                return false;
            }
        }
        return first1 == last1 &&  //1到头  2没到头  ---->2有剩余 返回true(说明1小2长)
                    first2 != last2
    }

    template<class T>
    inline const T &max(const T&a, const T& b){
        return a < b ? b :a;//返回大值 引用记录
    }

    template<class T,class Compare>
    inline const T &max(const T& a , const T &b, Compare comp){
        return comp(a,b) ? b : a;
    }

    template<class T>
    inline const T& min(const T& a, const T& b){
        return b < a ? b: a;
    }

    template<class T,class Compare>
    inline const T& min(const T& a , const T& b,Compare comp){
        return comp(b ,a) ? b:a;
    }

    //词典序比较
    template<class InputIterator1,class InputIterator2,class Compare>
    bool lexicographical_compare(InputIterator1 first1,InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                Compare comp){
        for(;first1 != last1 && first2!=last2;first1++,first2++){
            if(comp(*first1,*first2)){
                return true;
            }else if(comp(*first2,*first1)){
                return false;
            }
        }
        return first1 == last1 && 
                    first2 != last2;    //二序列有余  二长 返回true  比较大小默认<
    }

    //针对原始指针const unsigned char * 的全特化版本
    //字符串版本的比较
    inline bool lexicographical_compare(const unsigned char * first1,
                                        const unsigned char * last1,
                                        const unsigned char * first2,
                                        const unsigned char * last2){
        const size_t len1 = last1 - first1;
        const size_t len2 = last2 - first2;
        //先比较长度相同的段落
        const int result = memcmp(first1,first2,min(len1,len2));//依次比较
        //如果结果==0 ，说明前面相同长度出，每个字符都相同
            //只能比较长度了--->默认长度小的
        //如果结果!=0，如果小的个数多则为负值，所以返回result<0
        return result != 0 ? result < 0 :len1 < len2;
    }


    template<class InputIterator1,class InputIterator2>
    pair<InputIterator1,InputIterator2> mismatch(InputIterator1 first1,
                                                InputIterator1 last1,
                                                InputIterator2 first2,
                                                InputIterator2 last2){
                                                
        while(first1 ! last1 && (*first1 == *first2)){
            ++first1;
            ++first2;
        }
        return pair<InputIterator1,InputIterator2>(first1,first2);
    }
    //显然要求序列1长于序列2
    // mismathch 找到两个容器中，第一个不匹配所出现的位置 it1 和it2
    template<class InputIterator1,class InputIterator2,class BinaryPredicate>
    pair<InputIterator1,InputIterator2> mismatch(InputIterator1 first1,
                                                InputIterator1 last1,
                                                InputIterator2 first2,
                                                InputIterator2 last2,
                                                BinaryPredicate binary_pred){
        while(first1 ! last1 && binary_pred(*first1,*first2)){
            ++first1;
            ++first2;
        }
        return pair<InputIterator1,InputIterator2>(first1,first2);
    }

    template<class T>
    inline void swap(T & a, T & b){
        T temp = a;
        a = b;
        b = temp;
    }

    template<class InputIterator,class OutputIterator>
    struct __copy_dispatch{//仿函数对象
        //仿函数对象
        OutputIterator operator()(InputIterator first,InputIterator last,
                                OutputIterator result){
            //后续实现
            return __copy(first,last,result,
                            iterator_category_t<InputIterator>());
        }
    };

    template<class T>
    struct __copy_dispatch<T*,T*>
    {
        T* operator()(const T* first,const T* last,T*result ){
            using t = typename _type_traits<T>::has_trival_assignment_operator;
            return __copy_t(first,last,result,t());
        }
    };

    template<class InputIterator,class OutputIterator>
    inline OutputIterator copy(InputIterator first,InputIterator last,
    OutputIterator result){
        return __copy_dispatch<InputIterator,OutputIterator>()(
            first,last,result
        );//__copy_dispatch 是一个仿函数对象
    }
//针对指针偏特化
    inline char * copy(const char * first , const char * last,char * result){
        memmove(result,first,last-first);
        return result+(last-first);
    }

    //InputIterator
    template<class InputIterator,class OutputIterator>
    inline OutputIterator __copy(InputIterator first,InputIterator last,
    OutputIterator result,input_iterator_tag){
        for(;first!=last;++first,++result){
            *result = *first;//result 也在向后移动
        }
    }

    //RandomIterator
    template<class InputIterator,class OutputIterator,class Distance>
    inline OutputIterator __copy_d(InputIterator first,InputIterator last,
    OutputIterator result,random_access_iterator_tag){
        return __copy_d(first,last,result,
        difference_type_t<InputIterator>());
    }

    template<class  InputIterator , class OutputIterator,class Distance>
    inline OutputIterator __copy_d(InputIterator first, InputIterator last,
    OutputIterator result,Distance){
        for(Distance n = last - first ; n > 0; n--,first++ , last--){
            *result = * first;
        }
        return result;
    }

    




    
} // namespace MiniSTL
