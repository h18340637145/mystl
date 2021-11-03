#pragma once
#include "allocator.h"
#include "stl_list_iterator.h"
#include "uninitialized.h"

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

        list_node * get_node(){
            return list_node_allocator::allocate();
        }

        void put_node(list_node* p){
            list_node_allocator::deallocate(p);//释放p指向的那一块空间
        }

        list_node* create(const value_type&);
        void destory_node(list_node * p){
            destory(&p->data);//&(p->data)  ->优先级高
            put_node(p);
        }
    private:
        list_node *node;

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

        const_iterator begin() const noexcept{
            return const_iterator(node->next);
        }
        
    };


// 为了更方便的实现 list 模板类提供的函数，该模板类在构建容器时，
// 会刻意在容器链表中添加一个空白节点，并作为 list 链表的首个节点（又称头节点）。
    template<class T,class Alloc>
    inline void list<T, Alloc>::empty_initialized(){
        node = get_node();//分配一个类型大小的空间，前后指向都设置为自己，即就一个节点
        node->next = node;
        node->prev = node;
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

} // namespace MiniSTL
