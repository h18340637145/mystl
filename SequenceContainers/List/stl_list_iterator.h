#pragma once

#include <cstddef>  //ptrdiff_t
#include "stl_iterator.h"
#include "stl_list_node.h"

namespace MiniSTL
{
    template<class T>
    struct __list_iterator{
        //alias declarations
        using self = __list_iterator<T>;
        using link_type = __list_node<T> *;

        using iterator_category = bidirectional_iterator_tag;//链表是双向迭代器
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = ptrdiff_t;
        
        //data member   成员变量，迭代器本质就是其当前类型的指针，
        // 对于list数据结构，迭代器底层维护的就是一个listNode类型的指针
        link_type node; // 迭代器内部有一个指针，__list_node<T> *   指向list的节点

        //ctor
        __list_iterator(){}
        explicit __list_iterator(link_type x ): node(x){}

        //dtor
        //trival   迭代器的比较，本质是比较list数据结构底层的东西，底层就是一个listNode
        bool operator==(const self & rhs)const noexcept{
            return node == rhs.node;
        }

        bool operator!=(const self& rhs) const noexcept{
            return !(node == rhs.node);
        }

        //de reference
        //对迭代器解引用，就是取其内容，对于迭代器来说，就是取其底层的内容
        //对于list而言，就是取listNode的内容，即data
        reference operator*() const {
            return node->data;//(*node).data
        }
        //member access   成员存取的标准做法  &(operator*())
        pointer operator->() const{
            return &(operator*());
        }

        //increasement  前置 迭代器的底层向前移动->next  返回移动之后的自己
        self& operator++(){
            node = node->next;
            return *this;
        }
        //后置，返回原来的，需要临时变量，临时不可能返回引用，所以返回self对象即可
        self operator++(int){
            self temp = *this;
            ++(*this);  //self++
            return temp;
        }

        //decreasment
        self& operator--(){
            node = node->prev;
            return (*this);
        }
        self operator--(int){
            self temp = *this;
            --(*this);
            return temp;
        }



    };
    
} // namespace MiniSTL
