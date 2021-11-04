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
        self &operator+=(difference_type n){
            difference_type offset = n + (cur - first);
            
        }


    };
    
} // namespace MiniSTL
