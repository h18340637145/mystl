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
        void reserve_map_at_front(size_type  nodes_to_add = 1);
        void reserve_map_at_back(size_type nodes_to_add = 1);
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
        deque& operator=(const deque &);

    public://move operation
        deque(deque &&);
        deque & operator=(deque &&) noexcept;
    public:
        //getter
        const_iterator begin() const noexcept{
            return start;
        }
        const_iterator end() const noexcept{
            return finish;
        }
        const_iterator cbegin() const noexcept{
            return start;
        }
        const_iterator cend() const noexcept{
            return finish;
        }
        const_reverse_iterator crbegin() const noexcept{
            return const_reverse_iterator(finish);
        }
        const_reverse_iterator crend() const noexcept{
            return const_reverse_iterator(start);
        }
        const_reference operator[](size_type n) const noexcept{
            return start[static_cast<difference_type>(n)];
        }
        const_reference front() const noexcept{
            return *start;
        }
        const_reference back() const noexcept{
            return *finish;
        }
        size_type size() const noexcept{
            return finish - start;
        }
        bool empty() const noexcept{
            return finish == start;
        }

    public:
        //setter
        iterator begin() noexcept{
            return start;
        }
        iterator end() noexcept{
            return finish;
        }
        reverse_iterator rbegin() noexcept{
            return reverse_iterator(finish);
        }
        reverse_iterator rend() noexcept{
            return reverse_iterator(start);
        }
        reference operator[](size_type n){
            return start[static_cast<difference_type>(n)];
        }
        reference front() noexcept{
            return *start;
        }
        reference back() noexcept{
            return *(finish-1);
        }

    private:
        //push  pop 的辅助接口
        void push_back_aux(const value_type &);
        void push_front_aux(const value_type &);
        void pop_back_aux();
        void pop_front_aux();

    public:
        //push   pop
        void push_back(const value_type &);
        void push_front(const value_type &);
        void pop_back();
        void pop_front();

    private:
        //assign 的辅助接口  分配
        void fill_assign(size_type, const value_type &);
        template <class Integer>
        void assign_dispatch(Integer n, Integer val, _true_type){
            fill_assign(static_cast<size_type>(n), static_cast<value_type>(val));
        }

        template<class InputIterator>
        void assign_dispatch(InputIterator first, InputIterator last, _false_type){
            assign_aux(first, last, iterator_category_t<InputIterator>());
        }
        template<class InputIterator>
        void assign_aux(InputIterator, InputIterator, input_iterator_tag);
        template<class ForwardIterator>
        void assign_aux(ForwardIterator, ForwardIterator, forward_iterator_tag);

    public:
        //assign 分配
        void assign(size_type n, const value_type &val){
            fill_assign(n, val);
        }
        void assign(std::initializer_list<value_type> ils){
            assign(ils.begin(), ils.end());
        }
        template<class InputIterator>
        void assign(InputIterator first, InputIterator last){
            assign_dispatch(first, last, _is_integer_t<InputIterator>());
        }

        deque& operator=(std::initializer_list<value_type> ils){
            assign(ils.begin(), ils.end());
            return *this;
        }
    private:
        //insert辅助接口
        void fill_insert(iterator, size_type, const value_type &);
        template<class Integer>
        void insert_dispatch(iterator pos, Integer n, Integer val, _true_type){
            fill_insert(pos, static_cast<size_type>(n), 
                            static_cast<value_type>(val));
        }
        template<class InputIterator>
        void insert_dispatch(iterator pos, InputIterator first, InputIterator last, _false_type){
            range_insert_aux(pos, first, last, 
                            iterator_category_t<InputIterator>());
        }

        template<class InputIterator>
        void range_insert_aux(iterator, InputIterator, InputIterator,
                            input_iterator_tag);
        template<class ForwardIterator>
        void range_insert_aux(iterator, ForwardIterator, ForwardIterator,
                            forward_iterator_tag);
        iterator insert_aux(iterator, const value_type &);

        void insert_aux(iterator, size_type, const value_type &);
        template<class ForwardIterator>
        void insert_aux(iterator, ForwardIterator, ForwardIterator, size_type);
    public: 
    //insert
        iterator insert(iterator, const value_type &);
        iterator insert(iterator pos){
            return insert(pos, value_type());
        }
        void insert(iterator pos, size_type n, const value_type &val){
            fill_insert(pos, n, val);
        }
        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last){
            insert_dispatch(pos, first, last, _is_integer_t<InputIterator>());
        }

    public:
        //erase
        iterator erase(iterator);
        iterator erase(iterator, iterator);
        void clear();

    public:
        //resize()
        void resize(size_type, const value_type &);
        void resize(size_type new_size){
            resize(new_size, value_type());
        }
    public:
        void swap(deque &rhs) noexcept;
    };





//创建中心控制 数组
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
            deallocate_map(map, map_size);//map开始size大小
            map = nullptr;
            map_size = 0;
            throw;
        }
        start.set_node(nstart);
        finish.set_node(nfinish - 1);
        start.cur = start.first;
        finish.cur = finish.first + n % buffer_size() ; //bufsize  是512 / T
        //如果 n % buffer_size == 0 会多配置一个节点，此时cur指向该节点头部
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::fill_initialize(const value_type & val){
        map_pointer cur;
        //使用val填充每个值
        try {
            //为每个缓冲区设定初值
            // T** cur   start 是第一个中控点
            for(cur = start.node; cur < finish.node; ++cur){
                MiniSTL::uninitialized_fill(*cur, *cur + buffer_size(), val);
            }
            //最后一个缓冲区只设定至需要处
            MiniSTL::uninitialized_fill(finish.first, finish.cur, val);
        }
        catch(const std::exception& e) {
            destory(start, iterator(*cur, cur));
            throw;
        }
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void deque<T, Alloc>::range_initialize(InputIterator first,
                                            InputIterator last, 
                                            input_iterator_tag){

        initialize_map(0);//分配一块映射点
        try
        {
            for(; first != last ; ++first){
                push_back(*first);
            }
        }
        catch(const std::exception& e)
        {
            clear();
            throw;
        }
    }

    template<class T, class Alloc>
    template<class ForwardIterator>
    void deque<T, Alloc>::range_initialize(ForwardIterator first, 
                                            ForwardIterator last, 
                                            forward_iterator_tag){
        size_type n = MiniSTL::distance(first, last);
        initialize_map(n);// n / bufsize + 个中心点

        map_pointer  cur_node;
        try
        {
            for(cur_node = start.node; cur_node < finish.node; ++cur_node){
                ForwardIterator mid = first;
                MiniSTL::advance(mid, buffer_size());//跳到末尾
                MiniSTL::uninitialized_copy(first, last, *cur_node);
                first = mid;//当前索引区域的最后一个位置的后一位（下个索引区域的开头
            }
            MiniSTL::uninitialized_copy(first, last, finish.first);
        }
        catch(const std::exception& e)
        {
            destory(start, iterator(*cur_node, cur_node));
            throw;
        }
    }


//重新分配中央空间  添加几个点
    template<class T, class Alloc>
    inline void deque<T, Alloc>::reallocate_map(size_type nodes_to_add,
                                                bool add_at_front){
        size_type old_num_nodes = finish.node - start.node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;

        if(map_size > 2 * new_num_nodes){
            //某一端出现失衡，因此释放存储区完成重新中央分配，
            //规定新的nstart ， 若添加在前端则向后多移动n个单位
            new_nstart = map + (map_size - new_num_nodes) / 2 + 
                        (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start.node)
            {
                //整体前移
                MiniSTL::copy(start.node, finish.node + 1, new_nstart); 
            }
            else{ 
                //整体后移
                MiniSTL::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            }
        }else{
            size_type new_map_size = 
                map_size + MiniSTL::max(map_size, nodes_to_add) + 2;
            //分配新空间
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2+ 
                            (add_at_front ? nodes_to_add : 0);
            //拷贝原有内容
            MiniSTL::copy(start.node, finish.node + 1, new_nstart);
            //释放原map
            map_allocator::deallocate(map, map_size);
            //重新设定map
            map = new_map;
            map_size = new_map_size;
        }  
        //设定start与finish
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
        //注意并非new_num，接下来的设定转交其他函数处理
    }

    template<class T, class Alloc>
    inline deque<T, Alloc> &deque<T, Alloc>::operator=(const deque &rhs){
        const size_type len = size();
        if (&rhs != this)
        {   //this->size() 大  将rhs都拷贝过来，后面的都删除
            if (len >= rhs.size())
            {
                erase(MiniSTL::copy(rhs.begin(), rhs.end(), start),finish);
            }else{
                //this->size() 小    现将rhs的this size这么长复制到this中
                //再在this的末尾插入剩下的
                const_iterator mid = rhs.begin() + static_cast<difference_type>(len);
                MiniSTL::copy(rhs.begin(), mid, start);
                insert(finish, mid, rhs.end());//pos first end
            }
        }
        return *this;
    }

    // 1、reserve_map_at_back检查map空间是否足够，不够就申请新的空间，并将数据移动过去，
    // 并更新_M_impl中存储关键元素的信息；
    // 2、然后根据尾元素所在节点申请新的缓冲区空间；
    // 3、最后构造元素。
    template<class T, class Alloc>
    inline void deque<T, Alloc>::reserve_map_at_back(size_type nodes_to_add){
        // map_size - (finish.node - map + 1) == 后端剩余node个数
        //新加入点的数量大于后端剩余node的个数---需要重新分配一些空间
        if (nodes_to_add + 1 > map_size - (finish.node - map))
        {
            // 多分配nodestoadd个点
            reallocate_map(nodes_to_add, false);//false ---不是在front 进行 add
        }
    }

    template<class T, class Alloc>
    inline void deque<T, Alloc>::reserve_map_at_front(size_type nodes_to_add){
        //start.node - map ====前端剩余node的个数
        if (nodes_to_add > static_cast<size_type>(start.node - map))
        {
            reallocate_map(nodes_to_add, true);//true  表示在前端插入
        }
    }

    template<class T, class Alloc>
    inline typename deque<T, Alloc>::iterator deque<T, Alloc>::reserve_elements_at_front(size_type n){
        //"在前方储备元素"
        size_type vacancies  = start.cur - start.first;//第一个缓冲区中前方的剩余空间
        if (n > vacancies ) //如果即将储备的n个元素大于前方的元素个数， 那么就在前方插入 n-vac 个元素
        {
            new_elements_at_front(n - vacancies );
        }
        return start - static_cast<difference_type>(n);//start节点向前移动n个位置
        //deque_iterator  110行
    }
} // namespace MiniSTL
