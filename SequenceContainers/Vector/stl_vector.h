#pragma once

#include <cstddef>
#include "allocator.h"
#include "uninitialized.h"

namespace MiniSTL
{
    //use sub_allocator as default allocator
    template <class T,class Alloc = simpleAlloc<T>>
    class vector{
    public:
        using value_type = T;
        using pointer = value_type*;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = __reverse_iterator<iterator>;
        using const_reverse_iterator = __reverse_iterator<const_iterator>
        using reference = value_type&;
        using const_reference = const value_type &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    private://data member
        //iterator to indicate the vector's memory location
        iterator start;//指向容器内数据的指针
        iterator finish;
        iterator end_of_storage;

    private:  // allocate and construct aux functions
        using data_allocator = Alloc;//分配器类型

        void fill_initialize(size_type n, const value_type & value){
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }
    }
} // namespace MiniSTL
