#pragma once
#include <cstddef>
#include "stl_iterator.h"

namespace MiniSTL
{
    //缓冲区大小设定函数（在预设情况下传回可容纳的元素个数）
    //若n不为0，则传回n，表示由用户自定
    //若n为0，则才采用预设值 预设值根据sz（元素）大小而定

    inline size_t __deque_buf_size(size_t sz){
        return sz < 512 ? size_t(512 / sz) : size_t(1);
    }


    template<class T, class Ref, class Ptr>
    struct __deque_iterator
    {
        //alias declarartions   重命名
        using iterator = __deque_iterator<T, T &, T*>;
        using const_iterator = __deque_iterator<T, const T&, const T*>;
        using self = __deque_iterator;

        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using pointer = Ptr;
        using reference = Ref;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using map_pointer = T**;  //管控中心是指向指针的指针

        //data member 
        value_type * cur;//当前缓冲区的当前元素
        value_type * first;//当前缓冲区的头
        value_type * last;// 当前缓冲区的尾（含备用空间）
        map_pointer node;   //指向管控中心的当前节点  *node就是管控中心的第一个节点  
                        // 默认管控中心的最后一个应该是*node+buffer_size()

        static size_t buffer_size() {
            return __deque_buf_size(sizeof(value_type));
        }

        //ctor
        __deque_iterator() 
            : cur(nullptr), first(nullptr), last(nullptr),node(nullptr){}
        __deque_iterator(pointer x, map_pointer y)
            : cur(x), first(*y), last(*y + buffer_size()), node(y){}
        __deque_iterator(const iterator &rhs)
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node){}

    //建立管控中心的节点
    /* 使 node 跳跃到新的缓冲区 new_node */
        void set_node(map_pointer new_node){
            node = new_node;
            first = *new_node;
            last = first + static_cast<difference_type>(buffer_size());
        }

        //dereference
        reference operator*() const {
            return *cur;//对迭代器解引用  就是取其当前指向的内容 即*cur
        }

        pointer operator->() const {
            return &(operator*());  //对迭代器进行—>运算，就是获取*()的地址
        }

        self& operator++(){
            //++前置  返回+1后的结果*this
            cur++;
            if(cur == last){//如果++后抵达末尾，则
                set_node(node + 1);//在管控中心中当前节点之后加一个点
                cur = first;//此时的first指向新加入节点的首地址
            }
            return *this;
        }
        self operator++(int){
            self temp = *this;
            ++(*this);
            return temp;
        }
        self& operator--(){
            if (cur == first)
            {
                set_node(node - 1);//在管控中心当前节点的前面添加一个节点
                cur = last;//此时的last指向新加入管控中心节点的尾地址
            }
            --cur;
            return *this;
        }

        self operator--(int){
            self temp = (*this);
            --(*this);
            return temp;
        }

        //random access iterator   += 不只是1  可以是任意的数
        self &operator+=(difference_type n){//可加可减
            difference_type offset = n + (cur - first);  //偏移first多少  相当于index
            if(offset >= 0 
                && offset < static_cast<difference_type>(buffer_size())){ //在当前节点之内  
                //不需要跳转
                cur += n;
            }else{
                //offset 小于0 则必然需要跳转
                difference_type node_offset 
                = offset > 0 
                ? offset / static_cast<difference_type>(buffer_size())
                : -static_cast<difference_type>((-offset - 1) / buffer_size()) - 1;

                set_node(node + node_offset);
                cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
            }
            return *this;
        }

        self operator+(difference_type n){
            self temp = *this;
            return temp += n;
        }

        self& operator-=(difference_type n){
            return *this += -n;
        }

        self operator-(difference_type n){
            self temp = *this;
            return temp -= n;
        }

        reference operator[](difference_type n){
            return *(*this + n);
        }
    };

    template<class T, class Ref, class Ptr>
    inline bool operator==(const __deque_iterator<T, Ref, Ptr> &lhs,
                            const __deque_iterator<T, Ref, Ptr> &rhs){
        return lhs.cur == rhs.cur;
    }
    
    //compare with const
    template<class T, class RefL, class PtrL,class RefR, class PtrR>
    inline bool operator==(const __deque_iterator<T, RefL,PtrL> &lhs,
                            const __deque_iterator<T, RefR, PtrR> &rhs){
        return lhs.cur == rhs.cur;
    }

    template<class T, class Ref, class Ptr>
    inline bool operator!=(const __deque_iterator<T, Ref, Ptr> &lhs,
                            const __deque_iterator<T, Ref, Ptr> &rhs){
        return !(lhs == rhs);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator!=(const __deque_iterator<T, RefL,PtrL> &lhs,
                            const __deque_iterator<T, RefR, PtrR> &rhs){
        return !(lhs == rhs);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator<(const __deque_iterator<T, Ref, Ptr> &lhs,
                            const __deque_iterator<T, Ref, Ptr> &rhs){
        //同一个节点， 当前小的即可
        //不同节点的话，就是节点小的
        return (lhs.node == rhs.node) ? (lhs.cur < rhs.cur) : (lhs.node < rhs.node);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator<(const __deque_iterator<T, RefL, PtrL> &lhs,
                            const __deque_iterator<T, RefR, PtrR> &rhs){
        return (lhs.node == rhs.node) ? (lhs.cur < rhs.cur) : (lhs.node < rhs.node);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator>(const __deque_iterator<T, Ref, Ptr> &lhs,
                            const __deque_iterator<T, Ref, Ptr> &rhs){
        return rhs < lhs;
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator>(const __deque_iterator<T, RefL, PtrL> &lhs,
                            const __deque_iterator<T, RefR, PtrR> &rhs){
        return rhs < lhs;
    }

    template<class T, class Ref, class Ptr>
    inline bool operator<=(const __deque_iterator<T, Ref, Ptr> &lhs,
                            const __deque_iterator<T, Ref, Ptr> &rhs){
        return !(lhs > rhs);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator<=(const __deque_iterator<T, RefL, PtrL> &lhs,
                            const __deque_iterator<T, RefR, PtrR> &rhs){
        return !(lhs > rhs);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator>=(const __deque_iterator<T, Ref, Ptr> &lhs,
                            const __deque_iterator<T, Ref, Ptr> &rhs){
        return !(lhs < rhs);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator>=(const __deque_iterator<T, RefL, PtrR> &lhs,
                            const __deque_iterator<T, RefR, PtrR> &rhs){
        return !(lhs < rhs);
    }

    template<class T, class Ref, class Ptr>
    inline typename __deque_iterator<T, Ref, Ptr>::difference_type operator-(
        const __deque_iterator<T, Ref, Ptr> &lhs,
        const __deque_iterator<T, Ref, Ptr> &rhs
    ){// lhs - rhs  
        return typename __deque_iterator<T, Ref, Ptr>::difference_type(
            __deque_iterator<T, Ref, Ptr>::buffer_size() * (lhs.node - rhs.node - 1)
            + (lhs.cur - lhs.first) + (rhs.last - rhs.cur)

       //        =======c*******l  rhs    ------------     lhs   f*******c======   
       // ---- 的bufsize  + 两个** 的和
        );
    }
//
// lhs - rhs    
//
///* 重载 - 运算符，用于计算两个迭代器之间的距离 */
/* 注意：两个迭代器的 this 指针均不发生改变 */

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline typename __deque_iterator<T, RefL, PtrL>::difference_type operator-(
        const __deque_iterator<T, RefL, PtrL> &lhs,
        const __deque_iterator<T, RefR, PtrR> &rhs
    ){
        return typename __deque_iterator<T, RefL, PtrL>::difference_type(
            __deque_iterator<T, RefL, PtrL>::buffer_size() * (lhs.node - rhs.node -1)  //两个迭代器之间除头尾以外的缓冲区（他们是绝对饱和的）
            + (lhs.cur - lhs.first)     //左操作迭代器所指缓冲区内元素个数（该缓冲区可能未饱和）
            + (rhs.last - rhs.cur)      //右操作迭代器所指缓冲区内元素个数（该缓冲区可能未饱和）
        );
    }

    template<class T, class Ref, class Ptr>
    inline __deque_iterator<T, Ref, Ptr> operator+(
        ptrdiff_t n, const __deque_iterator<T, Ref, Ptr> &x
    ){
        return x + n;
    }
} // namespace MiniSTL
