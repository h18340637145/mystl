#pragma once

#include "allocator.h"
#include "deque_iterator.h"
#include "uninitialized.h"

namespace MiniSTL
{
    template<class T, class Alloc = simpleAlloc<T>>
    class deque{
    public:
        //alias declaration
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using iterator = __deque_iterator<T, T&, T*>;
        using reverse_iterator = MiniSTL::__reverse_iterator<iterator>;
        using const_iterator = __deque_iterator<T, const T&, const T*>;
        using const_reverse_iterator = MiniSTL::__reverse_iterator<const_iterator>;
    

    private:
        using map_pointer = pointer *;
        using node_allocator = simpleAlloc<value_type>;
        using map_allocator = simpleAlloc<pointer>;
    private:
        //data member
        iterator start ;//第一个节点
        iterator finish;
        map_pointer map;// 指向节点的指针 T** map
        size_type map_size;

    private:
        //aux_interface for node   节点的辅助接口
        value_type * allocate_node(){
            return node_allocator::allocate(__deque_buf_size(sizeof(value_type)));
        }

        void deallocate_node(value_type *p){
            //p 指向实际数组所在的地址  数组之后的bufsize那么大的空间重新分配
            node_allocator::deallocate(p, __deque_buf_size(sizeof(value_type)));
        }

        void create_nodes(map_pointer, map_pointer);

        void destory_nodes(map_pointer, map_pointer);

    private:
        //map的辅助接口
        void initialize_map(size_type);
        void deallocate_map(map_pointer p, size_type n){
            map_allocator::deallocate(p, n);
        }

        void reallocate_map(size_type, bool);
        void reverse_map_at_front(size_type  nodes_to_add = 1);
        void reverse_map_at_back(size_type nodes_to_add = 1);
        iterator reserve_elements_at_front(size_type);
        iterator reserve_elements_at_back(size_type);
        void new_elements_at_front(size_type);
        void new_elements_at_back(size_type);

    private:
        //构造器的辅助接口
        size_type initial_map_size() const noexcept{
            return 8U;
        }
        size_type buffer_size() const noexcept {
            return iterator::buffer_size();
        }
        void fill_initialize(const value_type &);
        template<class Integer>
        void initialize_dispatch(Integer n, Integer val, _true_type){
            initialize_map(static_cast<size_type>(n));
            fill_initialize(static_cast<value_type>(val));
        }

        template<class InputIterator>
        void initialize_dispatch(InputIterator first, InputIterator last,_false_type){
            range_initialize(first, last, iterator_category_t<InputIterator>());
        }

        template<class InputIterator>
        void range_initialize(InputIterator first, InputIterator last, input_iterator_tag);

        template<class InputIterator>
        void range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag);

    public:
        //ctor  dtor
        deque() : start(), finish(), map(nullptr), map_size(0){
            initialize_map(0);
        }
        explicit deque(size_type n) : start(), finish(), map(nullptr),map_size(0){
            initialize_map(n);
            fill_initialize(value_type());
        }
        deque(size_type n, const value_type &value) : start(), finish(), map(nullptr), map_size(0){
            initialize_map(n);
            fill_initialize(val);
        }

        template<class InputIterator>
        deque(InputIterator first, InputIterator last){
            initialize_dispatch(first, last, _is_integer_t<InputIterator>());
        }            
        deque(std::initializer_list<value_type> ils){
            range_initialize(ils.begin(), ils.end(), random_access_iterator_tag());
        }
        ~deque();

    public: //copy operations
        deque(const deque &rhs){
            initialize_map(rhs.size());
            MiniSTL::uninitialized_copy(rhs.begin(), rhs.end(), start);
        }

    public://move operation
        deque(deque &&);
        deque & operator=(deque &&) noexcept;
    
    };

    template<class T, class Alloc>
    void deque<T, Alloc>::create_nodes(map_pointer nstart, map_pointer nfinish){
        map_pointer cur;
        try
        {
            //为每一个节点配置空间
            for(cur = nstart; cur <= nfinish; ++cur){
                *cur = allocate_node();
            }
        }
        catch(const std::exception& e)
        {
            destory_nodes(nstart, cur);
            throw;
        }
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::destory_nodes(map_pointer nstart, map_pointer nfinish){
        for(map_pointer n = nstart ; n < nfinish; n++){
            deallocate_node(*n);//*n  表示实际数组所在的地址  传递过去使用 T*接受
            //两个 映射表中的指针之间
        }
    }

    //初始化映射区
    template<class T, class Alloc>
    void deque<T, Alloc>::initialize_map(size_type n){
        //所需节点数（整数则多配置一个）
        size_type num_nodes = n / buffer_size() + 1;
        //一个map至少管理8个节点，至多管理num_nodes + 2 个
        map_size = MiniSTL::max(initial_map_size() , num_nodes + 2);
        map = map_allocator::allocate(map_size);

        //令nstart与nfinish指向map所拥有的全部node的中部，以便日后扩充头尾
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes;
        try {
            create_nodes(nstart, nfinish);
        }
        catch(const std::exception& e) {
            deallocate_map(map, map_size);
            map = nullptr;
            map_size = 0;
            throw;
        }
        start.set_node(nstart);
        finish.set_node(nfinish - 1);
        start.cur = start.first;
        finish.cur = finish.first + n % buffer_size() ;
        //如果 n % buffer_size == 0 会多配置一个节点，此时cur指向该节点头部
        
    }

} // namespace MiniSTL
