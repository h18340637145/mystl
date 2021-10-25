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
        iterator finish;//size表示的
        iterator end_of_storage;//capacity表示的

    private:  // allocate and construct aux functions
        using data_allocator = Alloc;//分配器类型

        //填充并初始化
        void fill_initialize(size_type n, const value_type & value){
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;//结束存储的位置
        }
        template<class Integer>
        //pod类型
        void initialize_aux(Integer n,Integer val,_true_type){
            fill_initialize(static_cast<size_type>(n),static_cast<value_type>(val));
            //n个空间填充为val
        }

        template<class InputIterator>
        void initialized_aux(InputIterator first,InputIterator last, _false_type){
            //不是pod类型
            start = allocate_and_copy(first, last);
            finish = end_of_storage = start + MiniSTL::distance(first, last);
        }
        iterator allocate_and_fill(size_type n, const value_type &value){
            iterator result = data_allocator::allocate(n);
            MiniSTL::uninitialized_fill_n(result, n, value);//从res迭代器开始，向其中填充n个value值
            return result;//非pod类型返回头指针
        }

        template<InputIterator>
        iterator allocate_and_copy(InputIterator first, InputIterator last){
            size_type n = MiniSTL::distance(first, last);
            iterator result = data_allocator::allocate(n);//分配当前块*n空间
            MiniSTL::uninitialized_copy(first, last, result);
            return result;//根据底层，依次为迭代器所指类型进行构造，返回头
        }

        void deallocate() noexcept{
            //重分配
            if (start)
            {
                data_allocator::deallocate(start ,end_of_storage - start);
                //重新分配全部空间
            }
        }
        void destory_and_deallocate() noexcept{
            destory(start, finish);//pod类型无所谓   非pod类型  依次调用destory
            deallocate();
        }


    public:
        //swap  成员函数
        void swap(vector& ) noexcept;

    public:
        //ctor  dtor
        vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr){}
        explicit vector(size_type n){
            fill_initialize(n,value_type());//类类型 A() 空构造
        }
        vector(size_type n, const value_type& value){
            fill_initialize(n, value);
        }
        template<class InputIterator>
        vector(InputIterator first, InputIterator last){//使用迭代器构造  在区间的值
            initialize_aux(first, last, _is_integer_t<InputIterator>());//是否是pod
        }
        vector(std::initializer_list<T>);
        vector(const vector &);
        vector(vector &&)noexcept ; //右值移动
        ~vector(){
            destory(start, finish);
            deallocate();//重新分配全部空间
        }


    public://copy assignment operator=
        vector& operator=(const vector& );
    
    public://move assignment             这两个operator=   返回值都是vector的引用  传入的不同
    //右值传递参数为vector&&  双引用（右值引用）
        vector& operator=(vector &&) noexcept;

    public:
        //getter
        const_iterator begin() const noexcept{  return start;   } //返回的都是const_iterator 指向只读值
        const_iterator end() const noexcept{    return finish;  } 
        const_iterator cbegin() const noexcept{ return start;   }
        const_iterator cend() const noexcept{   return finish;  }
        const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator(finish);//就是反向迭代器  
            //底层current存放与之对应的正向迭代器
            //用末尾指针  初始化与之对应的正向迭代器
        }

        const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator(start); // 求end  传入start即可
        }
        // TODO:
        // have no empty check
        const_reference front() const noexcept{ return *begin();    }//返回第一个迭代器指向的值
        const_reference back() const noexcept{  return *(end()-1);  }

        // TODO:
        //have no boundary check and don 't use proxy
        const_reference operator[](const size_type n) const noexcept{
            return *(start + n);
        }

        size_type size() const noexcept{    return static_cast<size_type>(finish - start);  }
        size_type capacity() const noexcept {   return static_cast<size_type>(end_of_storage - start);  }

        bool empty() const noexcept{    return start == finish; }

    public:
        //setter
        iterator begin() noexcept { return start;   }
        iterator end() noexcept{    return end;     }
        reverse_iterator rbegin() noexcept { return reverse_iterator(finish);   }
        reverse_iterator rend() noexcept {return reverse_iterator(start);   }
        reference operator[](const size_type n) noexcept { return *(start + n);   }
        reference front() noexcept {    return *begin();    }
        reference back() noexcept { return *(end() - 1);    }
    public:
        //interface for size and capacity
        void resize(size_type, const value_type &);
        void resize(size_type new_size) {
            resize(new_size, value_type());//
            //重新规划大小， 每个空间使用value_type()初始化
        }

        void reserve(size_type);//分配新容量空间
        void shrink_to_fit() noexcept{
            vector temp(*this);
            swap(temp);
        }
        


    }
} // namespace MiniSTL
