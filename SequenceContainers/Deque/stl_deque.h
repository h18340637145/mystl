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

        template<class ForwardIterator>
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
    template<class T, class Alloc>
    inline typename deque<T,Alloc>::iterator 
    deque<T ,Alloc>::reserve_elements_at_back(size_type n){
        //在末尾储备元素
        size_type vacancies = finish.last - finish.cur -1 ; //最后一个映射区间剩余的元素个数
        if (n > vacancies)//如果需要储备的元素，比剩下空的个数多  再分配n-vacancise个空间，在后面
        {
            new_elements_at_back(n - vacancies);
        }
        return finish + static_cast<difference_type>(n);
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::new_elements_at_front(size_type new_elems){
        size_type new_nodes = (new_elems + buffer_size() - 1) / buffer_size();//新增管理节点的个数
        reserve_elements_at_front(new_nodes);
        size_type i;
        try
        {
            for(i = 1; i <= new_nodes; i++){
                *(start.node - i) = allocate_node();    
            }
        }
        catch(const std::exception& e)
        {
            for (size_type j = 0; j < i; j++)
            {
                deallocate_node(*(start.node - j));
                throw;
            }
        }
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::new_elements_at_back(size_type new_elems){
        size_type new_nodes = (new_elems + buffer_size() -1 ) / buffer_size();
        reserve_map_at_back(new_nodes);
        size_type i;
        try
        {
            for(i = 1; i <= new_nodes; i++){
                *(first.node + i) = allocate_node();
            }
        }
        catch(const std::exception& e)
        {
            for(size_type j = 0 ; j < i;j++){
                deallocate_node(*(finish.node + j));
            }
            throw ;
        }
    }

    template<class T, class Alloc>
    inline void deque<T, Alloc>::push_back_aux(const value_type & value){
        value_type value_copy = value;
        reserve_map_at_back();//若符合条件 则重新更换map
        *(finish.node + 1) = allocate_node();//配置新节点
        try
        {
            construct(finish.cur, value_copy);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;//更新finish.cur 为当前first  向后移动一次
        }
        catch(const std::exception& e)
        {
            deallocate_node(*(finish.node + 1));
            throw;
        }
    }

    template<class T, class Alloc>
    inline void deque<T, Alloc>::push_front_aux(const value_type& value){
        value_type value_copy = value;
        reserve_map_at_front(); //若符合条件则重新更换map
        *(start.node - 1) = allocate_node();//配置新节点
        try
        {
            start.set_node(start.node -1 );//开始节点前移
            start.cur = start.last -1 ;
            construct(start.cur , value);
        }
        catch(const std::exception& e)
        {
            ++start;
            deallocate_node(*(start.node-1));
            throw;
        }
    }
    //node表示管理节点本身   first指向其缓冲区的起始处，last指向缓冲区的末尾处
    //cur指向缓冲区当前元素的位置
    template<class T, class Alloc>
    inline void deque<T, Alloc>::pop_back_aux(){
        node_allocator::deallocate(finish.first);//释放最后一个管理区的开头
        finish.set_node(finish.node - 1);//最后一个管理节点向前移动
        finish.cur = finish.last - 1;//最后一个管理节点的最后一个位置变成当前位置
        destory(finish.cur);
    }

    template<class T, class Alloc>
    inline void deque<T, Alloc>::pop_front_aux(){
        destory(start.cur);
        node_allocator::deallocate(start.first);
        start.set_node(start.node +  1);//ministl中使用的finish.node+1 本人自行修改为start.node+1
        start.cur = start.first;
    }

    template<class T, class Alloc>
    typename deque<T, Alloc>::iterator deque<T, Alloc>::insert_aux(
        iterator pos, const value_type & val
    ){
        //pos位置插入val
        difference_type index = pos - start;// 插入点之前元素的个数
        value_type value_copy = val;
        if(static_cast<size_type>(index) < size() / 2){//前移，此时前移的代价小
            //插图见书
            push_front(front());//最前端加入哨兵以作为标志，注意此时start发生了改变
            iterator front1 = start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start + index;
            iterator pos1 = pos;
            ++pos1;
            MiniSTL::copy(front2 , pos1 , front1);//移动元素
        }else{
            //过程上述类似
            push_back(back());
            iterator back1 = finish;
            --back1;
            iterator back2 = back1;
            --back2;
            iterator pos  = start + index ;//pos
            MiniSTL::copy_backward(pos, back2 , back1);
        }
        *pos = value_copy;
        return pos;
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::fill_insert(iterator pos , size_type n, const value_type & val){
        //在pos处  插入n 个val的元素
        if(pos.cur == start.cur){
            iterator new_start = reserve_map_at_front(n);
            try
            {
                MiniSTL::uninitialized_fill(new_start, start, val);
                start = new_start;
            }   
            catch(const std::exception& e)
            {
                destory_nodes(new_start.node, start.node);  
            }
        }else if(pos.cur == finish.cur){
            //最后插
            iterator new_finish = reserve_map_at_back(n);
            try
            {
                MiniSTL::uninitialized_fill(finish, new_finish, val);
                finish = new_finish;
            }
            catch(const std::exception& e)
            {
                destory_nodes(finish + 1, new_finish.node + 1);
            }
        }else{
            insert_aux(pos, n , val);
        }
    }


//插入接口
    template<class T, class Alloc>
    void deque<T, Alloc>::insert_aux(iterator pos, size_type n, const value_type & val){
        //在pos处插入n个val  不是头尾
        const difference_type elems_before = pos - start;//插入点之前的元素个数
        size_type length = size();//deque的大小
        value_type value_copy = val;
        if(elems_before  < static_cast<difference_type>(length / 2)){
            //在前面插入
            iterator new_start = reserve_elements_at_front(n);//分配n个空间
            iterator old_start = start;
            pos = start + elems_before;//pos
            try
            {
                if(elems_before >= static_cast<difference_type>(n)){//**** --
                //插入元素的个数小于插入点之前的个数，说明在前面添加 的n个空间，放不下从start-pos这么多元素
                //现将n个元素移动到nstart
                    iterator start_n =  start + static_cast<difference_type>(n);
                    MiniSTL::uninitialized_copy(start , start_n, new_start);
                    start =  new_start;
                    MiniSTL::copy(start_n, pos, old_start);//在将剩下的几个元素移动到oldstart开始的地方，
                    //剩下的空间就是要插入的n个位置，使用fill进行填充即可
                    MiniSTL::fill(pos -  static_cast<difference_type>(n), pos, value_copy);
                }else {
                    //插入元素的个数多于插入点之前的个数
                    //添加了很多空间，现将pos之前的元素全部移动到new_start处
                    //后面的全部使用val填充即可
                    MiniSTL::uninitialized_copy_fill(start, pos, new_start, start, value_copy);
                    start = new_start;
                    MiniSTL::fill(old_start, pos, val);
                }
            }
            catch(const std::exception& e)
            {
                destory_nodes(new_start.node, start.node);
                throw;                
            }
        }else{
            //在后面插入
            iterator new_finish = reserve_elements_at_back(n);
            iterator old_finish = finish;
            const difference_type elems_after = static_cast<difference_type>(length) - elems_before;

            pos = finish - elems_after;
            try{
                if(elems_after >= static_cast<difference_type>(n)){
                    iterator finish_n = finish - static_cast<difference_type>(n);
                    MiniSTL::uninitialized_copy(finish_n,finish, finish);
                    finish= new_finish;
                    MiniSTL::copy_backward(pos, finish_n, old_finish);
                    MiniSTL::fill(pos, pos +static_cast<difference_type>(n), value_copy);

                }else{
                    MiniSTL::uninitialized_fill_copy(finish, pos + static_cast<difference_type>(n) , value_copy, pos, finish);
                    finish = new_finish;
                    MiniSTL::fill(pos, old_finish, value_copy);
                }
            }catch(std::exception   &){
                destory_nodes(finish.node + 1 , new_finish.node+1);
                throw;
            }
        }
    }


    template<class T, class Alloc>
    template<class ForwardIterator>
    void deque<T, Alloc>::insert_aux(iterator pos, ForwardIterator first, ForwardIterator last, size_type n){
        //插入一个迭代器区间的元素
        const difference_type elems_before = pos - start;
        size_type length = size();
        if(elems_before < static_cast<difference_type>(n)){
            //insert front
            iterator new_start = reserve_elements_at_front(n);
            iterator old_start = start;
            pos = start + elems_before;
            try {
                if(elems_before >= static_cast<difference_type>(n) ){
                    //插入的元素个数小于pos之前的元素个数
                    //先移动一部分，再移动一部分到oldstart
                    iterator start_n = start + static_cast<difference_type>(n);
                    MiniSTL::uninitialized_copy(start, start_n, new_start);
                    start = new_start;
                    MiniSTL::copy(start_n, pos, old_start);
                    MiniSTL::copy(first , last, pos - static_cast<difference_type>(n));
                }else{
                    ForwardIterator mid = first ;
                    MiniSTL::advance(mid, static_cast<difference_type>(n) - elems_before);
                    MiniSTL::uninitialized_copy_copy(start, pos, first, mid, new_start);
                    start = new_start;
                    MiniSTL::copy(mid, last, old_start);
                }
            }
            catch(std::exception &){
                destory_nodes(new_start.node, start.node);
                throw;
            }
        }else{
            //insert back
            iterator new_finish = reserve_elements_at_back(n);
            iterator old_finish = finish;
            const difference_type elems_after = static_cast<difference_type>(length) - elems_before;
            pos = finish - elems_after;
            try{
                if(elems_after >= static_cast<difference_type>(n)){
                    //插入位置后面 元素的个数要比插入元素个数的要多
                    iterator finish_n = finish - static_cast<difference_type>(n);
                    MiniSTL::uninitialized_copy(finish_n, finish, finish);
                    finish = new_finish;
                    MiniSTL::copy_backward(pos, finish_n, old_finish);
                    MiniSTL::copy(first, last, pos);//填入要插入的值
                }
                else{
                    //要插入的元素很多
                    ForwardIterator mid = first ;
                    MiniSTL::advance(mid , elems_after);
                    MiniSTL::uninitialized_copy_copy(mid,last,pos,finish,finish);
                    finish = new_finsh;
                    MiniSTL::copy(first , mid, pos);
                }
            }catch(std::exception &){
                destory_nodes(finish.node+1, new_finish.node+1);
                throw;
            }
        }
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void deque<T, Alloc>::range_insert_aux(iterator pos, InputIterator first, InputIterator last,input_iterator_tag){
        MiniSTL::copy(first, last, inserter(*this, pos));//在当前容器的pos位置
        //放入到inserter中pos之后？        返回一个insert_iterator
    }

    template<class T, class Alloc>
    template<class ForwardIterator>
    void deque<T, Alloc>::range_insert_aux(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag){

        size_type n = MiniSTL::distance(first, last);
        for(pos.cur == start.cur){
            iterator new_start = reserve_elements_at_front(n);
            try{
                MiniSTL::uninitialized_copy(first, last, new_start);
                start = new_start;
            }
            catch(std::exception &){
                destory_nodes(new_start.node, start.node);
                throw;
            }

        }else if(pos.cur == finish.cur  ){
            iterator new_finish = reserve_elements_at_back(n);
            try{
                MiniSTL::uninitialized_copy(first, last, finish);
                finish = new_finish;
            }catch(std::exception &){
                destory_nodes(finish.node + 1, new_finish.node+1);
                throw;
            }
        }else{
            insert_aux(pos, first, last, n);
        }
    }


    template<class T, class Alloc>
    inline deque<T, Alloc>::deque(deque &&rhs){
        initialize_map(0);
        if(rhs.map){//!=nullptr
            swap(rhs);
        }
    }

    template<class T, class Alloc>
    deque<T, Alloc> &deque<T, Alloc>::operator=(deque &&rhs) noexcept{
        clear();
        swap(rhs);//右值移动
        return *this;
    }

    template<class T, class Alloc>
    inline deque<T, Alloc>::~deque(){
        destory(start,finish);
        if(map)// !-nullptr
        {
            destory_nodes(start.node, finish.node+1);
            deallocate_map(map, map_size);
        }
    }

    template<class T, class Alloc>
    inline void deque<T,Alloc>::push_back(const value_type & value){
        //finish  的cur指向最后一个元素的下一个位置，因此if语句表征最少还有一个备用空间
        if(finish.cur != finish.last - 1){
            construct(finish.cur, value);
            ++finish.cur;
        }
        else
        {
            //最终缓冲区已无或仅剩一个空间
            push_back_aux(value);
        }
    }

    template<class T, class Alloc>
    inline void deque<T,Alloc>::pop_front(){
        if(start.cur != start.last - 1){
            destory(start.cur);
            ++start.cur;
        }else{
            pop_front_aux();
        }
    }
    template<class T, class Alloc>
    inline void deque<T,Alloc>::pop_back(){
        if(finish.cur != finish.first){
            destory(finish.cur);
            finish.cur--;
        }else{
            pop_back_aux();
        }
    }
    template<class T, class Alloc>
    inline void deque<T, Alloc>::clear(){
        //清空所有的node ，保留唯一缓冲区
        for(map_pointer node = start.node +1 ; node < finsih.node; node++){
            //对于每个缓冲区，就进行清空destory操作
            destory(*node, *node+buffer_size());//析构所有元素
            node_allocator::deallocate(*node, buffer_size());
        }
        if(start.node != finish.node){
            //存在头尾两个缓冲区
            //析构其中所有元素
            destory(start.cur, start.last);
            destory(finish.first, finish.cur);
            //保存头部(后续直接在头部缓冲区中加元素)，释放尾部
            node_allocator::deallocate(finish.first, buffer_size());
        }else{
            destory(start.cur, finish.cur);//利用finish.cur标记末尾
        }
        finish  = start;
    }
    template<class T, class Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::erase(iterator pos){
        iterator next = pos+1;
        difference_type index = pos - start;//清除点之前的元素个数
        if(index < size() / 2){
            //从前方移动次数少
            MiniSTL::copy_backward(start, pos, pos);//向后移动一位
            pop_front();
        }else{
            MiniSTL::copy(next, finish, pos);//向前移动一位
            pop_back();
        }
        return start + index;
    }

    template<class T, class Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::erase(iterator first, iterator last){
        if(first == start && last == finish){
            clear();
            return finish;//全删
        }
        else{
            difference_type n = last - first;//清除区间的长度
            difference_type elems_before = first - start ; //前方元素个数
            if(elems_before < (size() - n)/ 2){
                //前面个数少  删除之后，将前面的元素向后移动进行填补
                MiniSTL::copy_backward(start, first, last);
                iterator new_start = start + n;//标记新起点
                destory(start, new_start);//析构多余元素
                //释放多余缓冲区
                for(map_pointer cur = start.node; cur < new_start.node; ++cur){
                    node_allocator::deallocate(*cur, buffer_size());
                }
                start = new_start;
            }else{
                //前移开销低
                MiniSTL::copy(last, finish, first);
                iterator new_finish = finsh - n ;
                destory(new_finish,finish);
                //释放多余缓冲区
                for(map_pointer cur = new_finish.node + 1; cur <= finish.node;++cur){
                    node_allocator::deallocate(*cur, buffer_size());
                }
            }
            return start + elems_before;
        }
    }



} // namespace MiniSTL
