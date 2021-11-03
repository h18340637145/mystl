#pragma once
#include "allocator.h"
#include "stl_list_iterator.h"
#include "uninitialized.h"

// list有一个重要性质:插入操作(insert)和接合操作(splice)都不会造成原有的list迭代器失效.
// erase也只是造成指向的那个元素失效,不会对其他迭代器产生影响.
// SGI list不仅是一个双向链表,而且还是一个环装双向链表 所以它只需要一个指针,
// 便可以完整表现整个链表
// 让node可以指向放在尾部的一个空白结点

namespace MiniSTL
{
    template<class T, class Alloc = simpleAlloc<T>>
    class list{
    public:
        //alias
        //list<int,simpleAlloc<int>>
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const value_type*;
        using const_reference = const value_type&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using iterator = __list_iterator<T>;//迭代器类型对象
        using const_iterator = __list_const_iterator<T>;
        using reverse_iterator = MiniSTL::__reverse_iterator<iterator>;//双向迭代器
        using const_reverse_iterator = MiniSTL::__reverse_iterator<const_iterator>;

    private:
        //关于分配器的接口
        using list_node = __list_node<T>;
        using list_node_allocator = simpleAlloc<list_node>;//allocate(sizeof(list_node))

        //分配一个节点的空间
        list_node * get_node(){
            return list_node_allocator::allocate();
        }

        void put_node(list_node* p){
            list_node_allocator::deallocate(p);//释放p指向的那一块空间
        }

        list_node* create_node(const value_type&);
        void destory_node(list_node * p){
            destory(&p->data);//&(p->data)  ->优先级高
            put_node(p);
        }
    private:
        list_node *node;//node 指向尾部的一个空白节点

    private: 
        //aux_interface
        void empty_initialized();
        //move[first, last) before pos;
        void transfer(iterator position, iterator first, iterator last);

    public:
        list(){
            empty_initialized();
        }
        explicit list(size_type, const value_type &value = value_type());
        //不需要技巧，insert可以完成所有操作

        //构造器重载
        template<class InputIterator>
        list(InputIterator first, InputIterator last){
            empty_initialized();
            insert(begin(), first, last);//pos  first , last
        }
        list(std::initializer_list<value_type> il){
            empty_initialized();
            insert(begin(), il.begin(), il.end());
        }

        ~list(){
            clear();
            put_node(node);//重新分配node这块空间？
        }

    public:
        void swap(list &rhs) noexcept{
            MiniSTL::swap(node, rhs.node);
        }
    public:
    //copy ctor
        list(const list &);
        list& operator=(const list &) noexcept;
    private:
        //aux interface 辅助接口
        //assign  分配内存
        void fill_assign(size_type, const value_type &);

        template<class Integer>
        void assign_dispatch(Integer n, Integer val, _true_value){
            // 全部用val填充
            fill_assign(static_cast<size_type>(n), static_cast<value_type>(val));
        }

        template<class InputIterator>//迭代器区间  pod类型区分两个实现（类外）
        void assign_dispatch(InputIterator, InputIterator, _false_type);

    public:
        //assignment  分配内存
        void assign(size_type n, const value_type & val){
            fill_assign(n, val);
        }

        void assign(std::initializer_list<value_type> ils){
            assign_dispatch(ils.begin(), ils.end(), _false_type());
        }

        template<class InputIterator>
        void assign(InputIterator first, InputIterator last){
            assign_dispatch(first, last,_is_integer<InputIterator>());//迭代器区间，pod类型区分
        }


        list &operator=(std::initializer_list<T> ils){
            assign(ils.begin(), ils.end());
            return *this;
        }

    public:
        //move operation
        list(list &&rhs) noexcept{
            empty_initialized();//node变成一个节点
            MiniSTL::swap(node, rhs.node);//当前的已经置空，交换后将rhs置空
        }
        list& operator=(list && rhs) noexcept{
            clear();//将当前链表清理成一个头节点的空链表
            swap(rhs);//将rhs交换给当前list
            return *this;
        }


    public:
        //getter
        bool empty() const noexcept{
            return node->next == node;
        }
        size_type size() const noexcept{
            return MiniSTL::distance(cbegin(),cend());
        }

        //尾部空白节点的下一个节点
        const_iterator begin() const noexcept{
            return const_iterator(node->next);//使用node->next节点构造一个const_iterator对象
        }
        
        //尾部空白节点
        const_iterator end() const noexcept {
            return const_iterator(node);
        }
        const_iterator cbegin() const noexcept{
            return const_iterator(node->next);
        }
        const_iterator cend() const noexcept{
            return  const_iterator(node);
        }

        const_reverse_iterator crbegin() const noexcept{
            return const_reverse_iterator(cend());
        }
        const_reverse_iterator crend() const noexcept{
            return const_reverse_iterator(cbegin());
        }

        const_reference front() const noexcept{
            return *begin();
        }
        const_reference back() const noexcept{
            return *(--end());
        }

    public: 
    //setter
        iterator begin() noexcept{
            return iterator(node->next);
        }

        iterator end() noexcept{
            return iterator(node);
        }
        iterator rbegin() noexcept{
            return reverse_iterator(end());
        }
        iterator rend() noexcept{
            return reverse_iterator(begin());
        }
        reference front() noexcept{
            return *(begin());
        }
        reference back() noexcept{
            return *(--end());
        }

    public:
        void resize(size_type ,const value_type &val = value_type());

    private:
        //插入辅助接口
        void fill_insert(iterator, size_type, const value_type &);
        template<class Integer>//插入派遣
        void insert_dispatch(iterator pos, Integer n, Integer val, _true_type){
            // pos位置插入n个val
            fill_insert(pos, static_cast<size_type>(n), static_cast<value_type>(val));
        }
        template<class InputIterator>
        void insert_dispatch(iterator, InputIterator, InputIterator, _false_type);

    public:
        iterator insert(iterator pos) {
            return insert(pos, value_type());
        }  
        iterator insert(iterator, const value_type &);
        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last){
            //辅助插入有两个实现，一个pod  一个非pod
            insert_dispatch(pos, first, last,_is_integer_t<InputIterator>());
        }


    public:
        //erase
        iterator erase(iterator);
        iterator erase(iterator, iterator);
        void clear();

    public:
        //push  pop
        void push_front(const T& value) {
            insert(begin(),value);//头插一个节点  begin()前插  底层应该借用了尾节点
        }
        void push_back(const T& value){
            insert(end(), value);
        }
        void pop_front(){
            erase(begin());
        }
        void pop_back(){
            iterator temp = end();
            erase(--temp);
        }
    public:
        //other interface
        void unique();
        void splice(iterator pos, list &rhs){
            if (!rhs.empty())
            {
                transfer(pos, rhs.begin(), rhs.end());
            }
        }
        void splice(iterator, list& ,iterator);
        void splice(iterator pos, list& ,iterator first, iterator last){
            if (first != last)
            {
                transfer(pos, first, last);
            }
        }
        void merge(list& );
        void reverse();
        void sort();
        void remove(const T&);

    };

    template<class T, class Alloc>
    inline typename list<T, Alloc>::list_node *list<T, Alloc>::create_node(
        const T& value
    ){
        //分配一个当前类型 的空间
        list_node* p = get_node();
        try
        {
            // 使用value值对p->data进行构造
            construct(&p->data , value);
        }
        catch(const std::exception& e)
        {
            put_node(p);//置空这块空间
            throw;  
        }
        return p;
    }

// 为了更方便的实现 list 模板类提供的函数，该模板类在构建容器时，
// 会刻意在容器链表中添加一个空白节点，并作为 list 链表的首个节点（又称头节点）。
    template<class T,class Alloc>
    inline void list<T, Alloc>::empty_initialized(){
        node = get_node();//分配一个类型大小的空间，前后指向都设置为自己，即就一个节点
        node->next = node;
        node->prev = node;
    }

    template<class T,class Alloc>
    void list<T, Alloc>::fill_assign(size_type n, const value_type &val){
       //n全部使用val填充
        iterator i = begin();
        for(; i != end(); ++i, --n){
            *i = val;
        }
        if (n > 0)//n的个数大于list中节点数
        {
            //需要创建节点
            fill_insert(end(), n, val);
        }
        else{//n的个数小于等于list中节点数，将后面的擦除即可
            erase(i , end());
        }
    }

//辅助分配
    template<class T,class Alloc>
    template<class InputIterator>
    void list<T, Alloc>::assign_dispatch(InputIterator first, InputIterator last, _false_type){
        iterator start = begin();
        iterator finish = end();
        for(; start != finish&&first != last; ++start, ++first){
            *start = *first;
        }
        if(first == last){//赋值的迭代器区间没有当前链表大，将剩下的清除
            erase(start, finish);
        }else{//链表放不下，填充之后，将剩下的插入到末尾（末尾的后一个节点的前面)
            insert(finish, first, last);
        }
    }

    template<class T,class Alloc>
    inline void list<T, Alloc>::resize(size_type new_size, const value_type &val){
        iterator cur = begin();
        size_type len = 0;
        for(; cur != end() && len < new_size; ++cur, ++len);
        if (len == new_size)
        {
            erase(cur, end());  
        }else{
            // i == end()
            fill_insert(end(), new_size - len, val);
        }
    }
    
    template<class T,class Alloc>
    inline typename list<T, Alloc>::iterator list<T, Alloc>::insert(
        iterator position, const value_type & val){
        // using iterator = __list_iterator<T>   iterator是一个类  类声明对象position  对象中的成员node 是list_node*类型

        list_node *temp = create_node(val);//构建一个节点，仅仅含有val
        //插在pos的前面
        temp->next = position.node;
        temp->prev = position.node->prev;
        position.node->prev->next = temp;
        position.node->prev = temp;
        return iterator(temp);//保证下一次插入还是在pos位置的前面，
        // 相当于向前移动了一下，所以使用temp这个新构造的节点构造新的迭代器，
        // 原来的迭代器呢？  仍然指向原来的位置，并未失效
    }
   
   template<class T, class Alloc>
    void list<T, Alloc>::fill_insert(iterator position, size_type n, const value_type &val){
        for(size_type i = n ; i != 0; --i){
            position = insert(position, val);//pos 会不断前移，每次都是最前
        }
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void list<T, Alloc>::insert_dispatch(iterator position, InputIterator first ,
                                        InputIterator last, _false_type){
        //非pod类型   类类型，自己实现插入
        for(; first != last ; ++first){
            insert(position, *first);//始终在pos位置插入
        }
    }

    template<class T, class Alloc>
    inline typename list<T, Alloc>::iterator list<T, Alloc>::erase(
        iterator position
    ){
        list_node* next_node = position.node->next;
        list_node* prev_node = position.node->prev;
        next_node->prev = prev_node;
        prev_node->next = next_node;
        destory_node(position.node);
        return(iterator(next_node));
    }

    template<class T, class Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last){
        iterator res;
        while(first != last ){
            res = erase(first++);
        }
        return res;
    }

//将连续迭代器区间【）的元素迁移到特定的pos位置之前
    template<class T, class Alloc>
    inline void list<T, Alloc>::transfer(iterator position, iterator first, iterator last){
        if(position != last){
            last.node->prev->next = position.node;
            first.node->prev->next = last.node;
            position.node->prev->next = first.node;
            list_node *temp = position.node->prev;
            position.node->prev = last.node->prev;
            last.node->prev = first.node->prev;
            first.node->prev = temp;
            // last.node->prev->next = position.node;
            // position.node->prev = last.node->prev;
            // list_node* temp = position.node->prev;
            // list_node* start = first.node->prev;
            // temp->prev = first.node;
            // first.node->next = temp;

            // start->next = last.node;
            // last.node->prev = start;

        }
    }
    
    template<class T, class Alloc>
    list<T, Alloc>::list(size_type n, const value_type &value){
        empty_initialized();
        fill_insert(begin(), n , value);
    }

    template<class T,class Alloc>
    list<T, Alloc>::list(const list &rhs){
        empty_initialized();
        insert(begin(), rhs.begin(), rhs.end());
    }

    template<class T, class Alloc>
    inline list<T, Alloc> &list<T, Alloc>::operator=(const list &rhs) noexcept{
        list temp(rhs);
        swap(temp);
        return *this;
    }

    template<class T, class Alloc>
    void list<T, Alloc>::clear(){
        list_node* cur = node->next;
        while(cur != node){//链表转一圈，全部删除
            list_node *temp = cur;
            cur = cur->next;
            destory_node(temp);
        }
        node->next = node;//最后保留头结点
        node->prev = node;
    }

    template<class T, class Alloc>
    void list<T, Alloc>::remove(const T& value){
        iterator first = begin();
        iterator last = end();
        while (first != last)//迭代区间  查找与value相同的迭代器，将其删除
        {
            iterator next = first;
            ++next;
            if(*first == value){
                erase(first);
            }
            first = next;//first向后移动
        }
    }



//这个前提需要有序吧？
    template<class T, class Alloc>
    void list<T, Alloc>::unique(){//唯一化
        iterator first = begin();
        iterator last = end();
        if(first == last){
            return ;
        }
        iterator next = first;
        while (++next != last)
        {
            //遍历区间
            if (*first == *next){
                //
                erase(next);
                next = first;//删除一个点，回到初始
            }else{
                first = next;//向前移动一次
            }
        }
    }

    template<class T, class Alloc>
    inline void list<T, Alloc>::splice(iterator position, list &, iterator i){
        iterator j = i;
        ++j;
        // i==pos 自身无法插于自身之前
        //j == pos 已处于pos之前
        if (position == i || position == j)
        {
            return;
        }
        transfer(position, i , j);  //[i,j) 之间的数移动到pos之前 
        //本质上就是一个元素  i指向的那个
        // 移动到position位置之前
    }
    
    template<class T, class Alloc>
    void list<T, Alloc>::merge(list &x){
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        while(first1 != last1 && first2 != last2){
            if(*first2 < *first1){
                iterator next = first2;
                transfer(first1, first2, ++next);//2 < 1    2 1   增序
                first2 = next;
            }else{
                ++first1;
            }
            if (first2 != last2 )
            {
                transfer(last1, first2, last2);
            }
        }
    }

    template<class T, class Alloc>
    void list<T, Alloc>::reverse(){
        //empty  || size() == 1
        if (node->next == node || node->next->next == node){
            return ;
        }
        iterator first = begin();
        ++first;//begin 自身并不需要移动，他作为指示末尾元素的哨兵，
        //（确切的说，最终的begin.node->next == end.node)
        while (first != end()){
            iterator old = first;
            ++first;
            transfer(begin(), old, first);//123    21     321  
        }
    }

    template<class T, class Alloc>
    void list<T, Alloc>::sort(){
        if (node->next == node || node->next->next == node)
        {
            return ;
        }
        //数据缓冲区
        //counter[n]中最多存放2^(n+1)个元素，若大于则与counter[n+1]做归并
        list carry;
        list counter[64];
        int fill = 0;
        while (!empty())
        {
            carry.splice(carry.begin(),*this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty())
            {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill)
            {
                ++fill; 
            }
        }
        for (int i = 1; i < fill; i++)
        {
            counter[i].merge(counter[i-1]);
        }
        swap(counter[fill - 1]);
    }

    template<class T>
    inline void swap(list<T>& lhs, list<T> &rhs){
        lhs.swap(rhs);
    }

    template<class T>
    bool operator==(const list<T> &lhs, const list<T> &rhs){
        auto it1 = lhs.begin();
        auto it2 = rhs.begin();

        for(; it1 != lhs.end() && it2 != rhs.end(); ++it1, ++it2){
            if (* it1 != *it2)
            {
                return false;
            }
        }
        return it1 == lhs.end() && it2 == rhs.end();
    }

    template<class T>
    inline bool operator!=(const list<T> &lhs, const list<T> &rhs){
        return !(lhs == rhs);
    }

    template<class T>
    inline bool operator<(const list<T> &lhs, const list<T> &rhs){
        return MiniSTL::lexicographical_compare(lhs.begin(), lhs.end(),rhs.begin(), rhs.end());
    }

    template<class T>
    inline bool operator>(const list<T>& lhs, const list<T> &rhs){
        return !(rhs < lhs);
    }

    template<class T>
    inline bool operator>=(const list<T> &lhs, const list<T> &rhs){
        return !(lhs < rhs);
    }
} // namespace MiniSTL
