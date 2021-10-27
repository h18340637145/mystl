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
            iterator result = data_allocator::allocate(n);//分配当前块*n空间  result此时是头指针
            MiniSTL::uninitialized_copy(first, last, result);//放置到result中，result并为改变，如果有返回值 则为末尾
            return result;//根据底层，依次为迭代器所指类型进行构造 返回头
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
        void shrink_to_fit() noexcept{ //收缩到合适??
            vector temp(*this); 
            swap(temp);
        }
    public:// compare operator (member function)
        bool operator==(const vector&) const noexcept;
        bool operator!=(const vector& rhs) const noexcept{  //借用operator==
            return !(*this == rhs);
        }

    public://push pop
        void push_back(const value_type &);
        void pop_back(){
            --finish;
            destory(finish);
        }

    public: // erase
        iterator erase(iterator, iterator);//删除区间
        iterator erase(iterator position){//删除一个
            return erase(position, position + 1);
        }
        void clear(){//删除区间   全部头---尾
            erase(begin(),end());
        }
    private:
        //aux_interface for insert
        void insert_aux(iterator , const value_type&);//插入一个
        void fill_insert(iterator, size_type, const value_type &);//插入n个value
        //迭代器区间插入
        template<class InputIterator>
        void range_insert(iterator pos, InputIterator first, InputIterator last,
                        input_iterator_tag);
        
        template<class ForwardIterator>
        void range_insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag);

        template<class Integer> //迭代器处  插入n个值
        void insert_dispatch(iterator pos, Integer n, Integer value, _true_type){
            fill_insert(pos, static_cast<int>(n), value_type(value));
        }
        
        template<class InputIterator>//迭代器处   出入一个区间的值  正向迭代器 基类对象接受
        void insert_dispatch(iterator pos, InputIterator first, InputIterator last, _false_type){
            range_insert(pos, first, last, iterator_category_t<InputIterator>());
        }

    public:
    //insert
        iterator insert(iterator);
        iterator insert(iterator, const value_type &);
        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last){
            insert_dispatch(pos, first, last, _is_integer_t<InputIterator>()); 
            //是否是pod类型  重载自行选择
        }
    
    private:
        //aux_interface for assign   aux_interface---接口  assign----顺次拷贝一个对象到另一个对象
        void fill_assign(size_type, const value_type&);//n个数  都填写val
        template<class Integer>
        void assign_dispatch(Integer n, Integer val, _true_type){
            fill_assgin(static_cast<size_type>(n), static_cast<value_type>(val));
        }
        template<class InputIterator>
        void assign_dispatch(InputIterator first, InputIterator last, _false_type){
            //不是pod类型
            assign_aux(first, last, iterator_category_t<InputIterator>());
        }

        //下面类外再实现   将first，last区间值  放置到begin之后   
        template<class InputIterator> // 
        void assign_aux(InputIterator first, InputIterator last, input_iterator_tag);

        template<class ForwardIterator>//特化迭代器
        void assign_aux(ForwardIterator first, ForwardIterator last,forward_iterator_tag);

    public://assign----顺次拷贝
        void assign(size_type n, const value_type &value){
            fill_assign(n, val);
        }
        template<class InputIterator>
        void assign(InputIterator first, InputIterator last){
            assign_dispatch(first, last, _is_integer_t<InputIterator>());//是否是pod
        }
        void assign(std::initializer_list<value_type> ils){
            assign(ils.begin(),ils.end());
        }

        vector & operator=(std::initializer_list<value_type> ils){
            assign(ils);
            return *this;
        }
    };

//  aux_interface---接口   insert实现
    template<class T,class Alloc>
    void vector<T,Alloc>::insert_aux(iterator position, const value_type & value){
        if(finish != end_of_storage){//仍然有空间
            construct(finish, *(finish - 1));//末尾出构造一个
            ++finish;
            value_type value_copy = value;//stl copy in copy out
            //整体向后移动一次
            MiniSTL::copy_backward(position, finish - 2, finish - 1);
            //相当于将position后面 后面的元素都向后移动一个单位
            //finish指向最后一个元素后面的那个位置  finish-2  相当于未添加前的finish-1 即末尾元素
            //finish-1即未添加前的end() 将[end()-1  到position)这个反向区间的数  一次移动到[end(),开头的这个地方
            *position = value_copy;
        }else{
            //expand   扩容
            const size_type = old_size = size();
            const size_type new_size =   //不是0  就扩一倍  否则  就是1
                old_size() ? 2 * old_size : 1;//new_cap = 2 * old_cap
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;//扩容之后，，没有元素 
            try {
                new_finish = MiniSTL::uninitialized_copy(
                    start, position, new_start
                );//复制前一段，position)
                construct(new_finish,value);
                ++new_finish;
                // [postition,..
                new_finish = MiniSTL::uninitialized_copy(
                    position, finish, new_finish
                );//拷贝第二段 ， new_finish)    指向末尾
            }catch(std::exception&){
                //commit or callback
                //出现异常的时候，销毁新创建的扩容空间，重新分配一块new_size大小的空间
                destory(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw; 
            }

            destory_and_deallocate();  //销毁start ， finish(原空间)  重新分配newstart，endof-start
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::range_insert(iterator pos, InputIterator first,
                                    InputIterator last, input_iterator_tag){
        for(; first != last; ++first){
            //从pos开始插入一个迭代器区间的元素
            //开始的pos  [pos
            pos = insert(pos, *first);
            ++pos;
        }
    }
    template<class T, class Alloc>
    template<class ForwardIterator>
    void vector<T,Alloc>::range_insert(iterator position, ForwardIterator first,
                                    ForwardIterator last, forward_iterator_tag){
        if(first != last){
            size_type n = MiniSTL::distance(first, last);
            if(static_cast<size_type>(end_of_storage - finish) >= n){
                //当前vector容量可以容纳新插入的n个元素，那么开始插入
                const size_type elem_after = finish - position;
                iterator old_finish = finish;
                if(elems_after > n){//插入的值少与主区间 防止重叠  在其中挖出n个空间
                    MiniSTL::uninitialized_copy(finish - n, finish, finish);  // finish-n  放置到finish中
                    // 移出finish区间
                    finish += n; 
                    // 末尾移动至 oldfinish处
                    MiniSTL::copy_backward(position, old_finish - n, old_finish);
                    //插入
                    MiniSTL::copy(position, position + n, position);//末尾这个不是first?
                }else{
                    ForwardIterator mid = first;
                    advance(mid, elems_after);
                    MiniSTL::uninitialized_copy(mid, last, finish);
                    finish += n - elems_after;
                    MiniSTL::uninitialized_copy(position, old_finish, finish);
                    finish += elem_after;
                    MiniSTL::copy(first, mid, position);
                }
            }else{
                //expand 扩容
                // first == last 
                const size_type old_size = size();//原来的大小
                const size_type new_size = old_size + MiniSTL::max(old_size, n);//扩容到可以装进去或者2倍大小
                iterator new_start = data_allocator::allocate(new_size);//分配新的空间
                iterator new_finish = new_start;
                try
                {
                    new_finish = MiniSTL::uninitialized_copy(start, position, new_start);//前一半
                    new_finish = MiniSTL::uninitialized_copy(first, last, new_finish);//插入段
                    new_finish = MiniSTL::uninitialized_copy(position, finish, new_finish);//后一半
                }
                catch(std::exception &)
                {
                    destory(new_start, new_finish);//如果失败，将新分配的空间全部销毁
                    data_allocator::deallocate(new_start, new_size);//重新分配这段空间
                    throw;  //抛出异常
                }
                destory_and_deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + new_size;
            }
        }
    }

    template <class T, class Alloc>
    inline void vector<T,Alloc>::swap(vector &rhs) noexcept{
        //交换两个vector
        MiniSTL::swap(start, rhs.start);//头尾capacity
        MiniSTL::swap(finish,rhs.finish);
        MiniSTL::swap(end_of_storage, rhs.end_of_storage);
    }

    template<class T, class Alloc>
    inline vector<T,Alloc>::vector(vector &&rhs) noexcept{
        //右值移动
        start = rhs.start;
        finish = rhs.finish;
        end_of_storage = rhs.end_of_storage;
        //本质是复制，并将原来的置空  因为是pod类型  只能复制  对象类型可能会使用move
        rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
    }

    template<class T, class Alloc>
    inline vector<T,Alloc>& vector<T,Alloc>::operator=(const vector& rhs){
        //operator=  复制vector
        vector temp(rhs);//构造一个rhs相同的对象
        swap(temp);//将临时的置换给this  赋值  不销毁
        return *this; //再返回this
    }

    template<class T,class Alloc>
    inline vector<T,Alloc> & vector<T,Alloc>::operator=(vector &&rhs) noexcept{
        //移动对象   将原有的对象移动给当前对象
        if (this !=rhs)
        {
            //不相同是时候才移动
            destory_and_deallocate();//销毁start，finish   重新分配空间start，end_of_storage-start
            start = rhs.start;
            finish = rhs.finish;
            end_of_storage = rhs.end_of_storage;
            rhs.end_of_storage = rhs.finish = rhs.start = nullptr;  //移动之后置空
        }
        return *this;
    }

    template<class T,class Alloc>
    inline void vector<T,Alloc>::resize(size_type new_size, const value_type &value){
        //重新设置大小
        if(new_size < size()){
            erase(begin() + new_size, end());//后面的删除即可  迭代器区间删除
        }else{
            //新空间大 在末尾插入  new_size - size 个value
            fill_insert(end(), new_size - size(), value);
        }
    }

    template<class T, class Alloc>
    inline void vector<T,Alloc>::reserve(size_type new_capacity){
        //重新规划容量
        if(new_capacity <= capacity()){
            //缩小容量
            return;
        }
        T* new_start = data_allocator::allocate(new_capacity);
        T* new_finish = MiniSTL::uninitialized_copy(start, finish, new_start);
        destory_and_deallocate();//删除start，finish  重新分配start， end_of_storage - start
        start = new_start;
        finish = new_finish;
        end_of_storage = start + new_capacity;
    }

    template<class T, class Alloc>
    bool vector<T, Alloc>::operator==(const vector & rhs)const noexcept{
        if (size() != rhs.size()) {
            return false;
        }else{//长度相同了
            iterator ptr1 = start;
            iterator ptr2 = rhs.start;
            for(;ptr1 != finish && ptr2 != rhs.finish; ++ptr1, ++ptr2){
                if(*ptr1 == *ptr2){
                    return false;//有一个值不相同   false
                }
            }
            //长度相同且各元素相同
            return true;
        }
    }

    template<class T, class Alloc>
    inline void vector<T,Alloc>::push_back(const value_type &value){
        if(finish != end_of_storage){
            //容量够用，可以插入
            construct(finish, value);
            ++finish;
        }else{
            insert_aux(end(), value);//容量不够，aux中会考虑扩容问题
        }
    }
    
    //返回一个迭代器
    //删除一个迭代器区间
    template<class T,class Alloc>
    inline typename vector<T,Alloc>::iterator vector<T, Alloc>::erase(
        iterator first, iterator last
    ){
        iterator i = MiniSTL::copy(last, finish, first);//覆盖区间，i保存最后元素
        destory(i, finish);     //删除后面
        finish -= (last - first);
        return first;// fist在copy时并不发生变化，指向被删除区间最后一个位置的下一块空间
        // 返回被删除区间的后面的元素
    }

    template<class T,class Alloc>
    void vector<T, Alloc>::fill_insert(iterator position, size_type n, const value_type &value){
        // 在pos之后插入n个val
        if (n != 0)//不是插入0个
        {
            if(static_cast<size_type>(end_of_storage - finish) >= n){
                //不需要扩展   空间足够
                value_type value_copy = value;
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if(elems_after > n){
                    MiniSTL::uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    //把postion处的元素移动到old_finish-n处  目标的末尾的old_finish
                    MiniSTL::copy_backward(position, old_finish - n, old_finish);
                    MiniSTL::fill(position, position + n,value_copy);//填充元素
                }else{
                    //看不懂
                    //要插入很多数
                    MiniSTL::uninitialized_fill_n(finish, n - elems_after, value_copy);//多出来的一定是value_copy值
                    finish += n - elems_after;
                    MiniSTL::uninitialized_copy(position, old_finish, finish);//原来元素所在的位置因为插入，相当于整体被移动到最后
                    finish += elems_after;
                    MiniSTL::fill(position, old_finish, value_copy);//pos处插入剩下的val
                }
            }else{
                //扩容
                const size_type old_size = size();
                const size_type new_size = old_size + MiniSTL::max(old_size, n);
                iterator new_start = data_allocator::allocate(new_size);
                iterator new_finish = new_start;
                try
                {
                    new_finish = MiniSTL::uninitialized_copy(start, position, new_start);//返回头指针
                    new_finish = MiniSTL::uninitialized_fill_n(new_finish, n, value);
                    new_finish = MiniSTL::uninitialized_copy(position, finish, new_finish);
                }
                catch(std::exception&)
                {
                    destory(new_start, new_finish);//扩容失败，销毁  重分配大空间 抛异常
                    data_allocator::deallocate(new_start, new_finish);
                    throw;
                }
                destory_and_deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + new_size;
            }
        }
    }
    template<class T, class Alloc>//一个参数，调用两个参数的
    inline typename vector<T,Alloc>::iterator vector<T, Alloc>::insert(
        iterator position
    ){
        return insert(position, value_type());//当前vector存储类型的
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(
        iterator position, const value_type &value //const type  只读变量 引用 只读原数据
    ){
        size_type n = position - begin();//插入位置
        //容量足够且插入位置在末尾
        if (finish != end_of_storage && position == end())
        {
            construct(finish, value);
            ++finish;
        }else{
            //普通插入，判断是否扩容，不用扩容的时候，整体向后移动，在pos位置插入即可
            insert_aux(position, value);
        }
        return begin() + n;//插入点保持不变，后续如果再插  还是在这里
    }
    
    //全都使用val来填充
    template<class T, class Alloc>  // 成员函数不内敛  应该是超过行了
    void vector<T, Alloc>::fill_assign(size_type n, const value_type &val){
        if(n > capacity()){
            //容量不够，扩容
            vector<T, Alloc> temp(n, val);
            temp.swap(*this);//又缩回去了？
        }else if(n > size()){
            //容量够用，
            MiniSTL::fill(begin(), end(), val);//直接填充
            finish = MiniSTL::uninitialized_fill_n(finish, n - size(), val);//后面填充
        }else{
            //删除 fill_n返回填充后的末尾  从此处到vector末尾都删除即可
            erase(MiniSTL::fill_n(begin(), n, val), end());//前面都填充，后面删掉
        }
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::assign_aux(InputIterator first, InputIterator last, input_iterator_tag){
        iterator cur = begin();
        for(; first != last && cur != end(); ++cur, ++first){
            *cur = *first;
        }
        if(cur == last){
            erase(cur, end());
        }else{
            insert(end(), first, last);//从end() 开始插入first，end区间的元素
        }
    }
    
    template<class T,class Alloc>
    template<class ForwardIterator>
    void vector<T,Alloc>::assign_aux(ForwardIterator first, ForwardIterator last, forward_iterator_tag){
        size_type len = MiniSTL::distance(first, last);
        if (len > capacity())
        {
            //分配空间
            iterator temp = allocate_and_copy(first, last);//temp为头处
            destory_and_deallocate();
            start = temp;
            end_of_storage = finish = start + len;
        }
        else if(size() >= len){
            iterator new_finish = MiniSTL::copy(first, last, start);
            destory(new_finish, finish);
            finish = new_finish;//first last之外的数据全部删除
        }else{

            ForwardIterator mid = first;
            MiniSTL::advance(mid, size());
            MiniSTL::copy(first, mid ,start);//前一半存在的移动过来
            finish = MiniSTL::uninitialized_copy(mid, last, finish);//capacity空间非size空间移动过来
        }
    }
    template<class T,class Alloc>
    inline vector<T,Alloc>::vector(const vector &rhs){
        //copy ctor
        start = allocate_and_copy(rhs.begin(), rhs.end());//返回头
        finish = end_of_storage = start + rhs.size();
    }

    template<class T,class Alloc>
    inline vector<T, Alloc>::vector(std::initializer_list<T> il){
        start = allocate_and_copy(il.begin(), il.end());
        finish = end_of_storage + il.size();
    }

    // ==
    // !=
    // <
    // >
    // <=
    // >=
    // swap
    //全局函数
    template<class T, class Alloc>
    inline bool operator==(const vector<T> &lhs,
                            const vector<T> &rhs){
        return lhs.operator==(rhs);
    }

    template<class T, class Alloc>
    inline bool operator!=(const vector<T> &lhs,
                            const vector<T> &rhs){
        return !(lhs == rhs);
    }

    template<class T>
    inline bool operator<(const vector<T> &lhs, const vector<T> &rhs){
        return MiniSTL::lexicographical_compare(lhs.begin(), lhs.end(),rhs.begin(), rhs.end());
    }
    template<class T>
    inline bool operator>(const vector<T> &lhs, const vector<T> &rhs){
        return rhs < lhs;
    }

    template<class T>
    inline bool operator<=(const vector<T>& lhs, const vector<T> &rhs){
        return !(lhs > rhs);
    }

    template<class T>
    inline bool operator>=(const vector<T>& lhs, const vector<T> &rhs){
        return rhs <=lhs;
    }

    template<class T,class Alloc>
    inline void swap(const vector<T, Alloc> &lhs, const vector<T,Alloc> &rhs){
        lhs.swap(rhs);
    }
} // namespace MiniSTL
