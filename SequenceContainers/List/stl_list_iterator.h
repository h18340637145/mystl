#pragma once

#include <cstddef>  //ptrdiff_t
#include "stl_iterator.h"
#include "stl_list_node.h"

namespace MiniSTL
{
    template<class T>
    struct __list_iterator
    {
        //alias declarations
        using self = __list_iterator<T>;
        using link_type = __list_node<T> *;

        using iterator_category = bidirectional_iterator_tag;//链表是双向迭代器
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = ptrdiff_t;
        
        //data member 
        link_type node; // 迭代器内部有一个指针，__list_node<T> *   指向list的节点

        //ctor
        __list_iterator(){}
        explicit __list_iterator(link_type x ): node(x){}

        //dtor
        //trival

        bool operator==(const self & rhs)const noexcept{
            return node == rhs.node;
        }

        bool operator!=(const self& rhs) const noexcept{
            return !(node == rhs.node);
        }

        //de reference
        reference operator*() const {
            return node->data;//(*node).data
        }
        //member access   成员存取的标准做法  &(operator*())
        pointer operator->() const{
            return &(operator*());
        }

        //increasement
        self& operator++(){
            node = node->next;
            return *this;
        }
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
