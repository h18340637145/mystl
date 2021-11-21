#pragma once

#include "allocator.h"
#include "rb_tree_iterator.h"
#include "stl_algobase.h"
#include "stl_function.h"


namespace MiniSTL
{
    //红黑树
    template<class Key , class Value, class KeyOfValue, class Compare, class Alloc = simpleAlloc<Value>>
    class rb_tree{
    private:
        //internal alias declarations
        using base_ptr = __rb_tree_node_base*;
        using rb_tree_node = __rb_tree_node<Value>;
        using rb_tree_node_allocator = simpleAlloc<__rb_tree_node>;
        using color_type = rb_tree_color_type;

    public:
        //basic type declarations
        using key_type = Key;
        using value_type = Value;
        using pointer = value_type *;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using link_type = rb_tree_node*;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    public:
        //Iterator declarations     
        using iterator = rb_tree_iterator<value_type, reference, pointer>;
        using const_iterator = rb_tree_iterator<value_type, const_reference, const_pointer>;
        using reverse_iterator = __reverse_iterator<iterator>;
        using const_reverse_iterator = __reverse_iterator<const_iterator>;

    private:
    //分配空间
        link_type get_node(){
            return rb_tree_node_allocator::allocate();
        }
    //重新分配空间
        void put_node(link_type p){
            rb_tree_node_allocator::deallocate(p);
        }
    //构造节点
        link_type create_node(const value_type &value){
            link_type temp = get_node();
            try
            {
                construct(&temp->value_field, value);   
            }
            catch(const std::exception& e)
            {
                put_node(temp);
            }
            return temp;
        }
    //复制节点
        link_type clone_node(link_type){
            link_type temp = create_node(p->value_field);
            temp->color = p->color;
            temp->left  = nullptr;
            temp->right = nullptr;
            return temp;
        }
    //析构+释放空间
        void destory_node(link_type p){
            destory(&p->value_field);//析构
            put_node(p);//释放空间
        }
    private:
        //成员变量
        size_type node_count;//用节点数量表示树的大小
        link_type header;//root的父亲  实现技巧
        Compare key_compare;

    private:
        // getter and  setter
        //header 和root 互为parent
        link_type &root() const noexcept{
            return reinterpret_cast<link_type&>(header->parent);
        }
        //根节点的左
        link_type& leftmost() const noexcept{
            return reinterpret_cast<link_type&>(header->left);
        }
        //根节点的右
        link_type& rightmost() const noexcept{
            return reinterpret_cast<link_type&>(header->right);
        }

        //一般节点的快速访问与设定
        static link_type& left(link_type p){
            return reinterpret_cast<link_type&>(p->left);
        }

        static link_type& right(link_type p){
            return reinterpret_cast<link_type&>(p->right);
        }
        static reference & parent(link_type p){
            return reinterpret_cast<link_type&>(p->parent);
        }
        static reference & value(link_type p){
            return p->value_field;
        }
        static const Key & key(link_type p){
            return KeyOfValue()(value(p));
        }
        static color_type &color(link_type p){
            return p->color;
        }


        //basenode  的快速访问与设定
        static link_type &left(base_ptr p){
            return reinterpret_cast<link_type &>(p->left);
        }
        static link_type& right(base_ptr p){
            return reinterpret_cast<link_type &>(p->right);
        }

        static link_type& parent(base_ptr p){
            return reinterpret_cast<base_ptr&>(p->parent);
        }

        static reference &value(base_ptr ){
            return reinterpret_cast<link_type>(p)->value_field;
        }

        static const Key &key(base_ptr p){
            return KeyOfValue()(value(reinterpret_cast<link_type>(p)));
        }
        static color_type & color(base_ptr p){
            return reinterpret_cast<link_type>(p)->color;
        }

        //求取极值（转交node_base）
        static link_type minimum(link_type p){
            return reinterpret_cast<link_type>(__rb_tree_node_base::minium(p));
        }
        static link_type maximum(link_type p){
            return reinterpret_cast<link_type>(__rb_tree_node_base::maximum(p));
        }

    private:
        //分配辅助接口
        link_type copy(link_type, link_type);
        void empty_initialize(){
            header = get_node();
            color(header) = rb_tree_red;
            root() = nullptr;
            rightmost() = header;
            leftmost() = header;
        }

    private:
    //旋转，平衡
        void rb_tree_rotate_left(base_ptr, base_ptr &);
        void rb_tree_rotate_right(base_ptr, base_ptr &);
        void rb_tree_rebalance(base_ptr , base_ptr &);
        base_ptr rb_tree_rebalance_for_earse(base_ptr , base_ptr& , base_ptr &, base_ptr);

    public :
        //ctor  dtor
        re_tree(): node_count(0), key_compare(){ empty_initialize(); }
        explicit rb_tree(const Compare& comp) : node_count(0), key_compare(comp){
            empty_initialize();
        }
        ~rb_tree(){
            clear();
            put_node(header);
        }

    public:
        // copy_operation
        
    };
} // namespace MiniSTL
