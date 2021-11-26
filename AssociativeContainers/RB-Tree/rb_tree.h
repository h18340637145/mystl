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
        base_ptr rb_tree_rebalance_for_erase(base_ptr , base_ptr& , base_ptr &, base_ptr);

    public:
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
        rb_tree(const rb_tree &rhs): node_count(0), key_compare(rhs.key_compare){
            if(!rhs.root()){
                //空节点  没有header
                empty_initialize();
            }else{
                header = get_node();//分配一块header大小的空间
                //返回引用
                color(header)  = rb_tree_red;
                root() = copy(rhs.root(), header);
                leftmost() = minimum(root());
                rightmost() = maximum(root());//从根开始，找到最右下的点
            }
            node_count = rhs.node_count;
        }
        rb_tree &operator=(const rb_tree &);
    public:
        //移动move
        rb_tree(rb_tree &&rhs) noexcept{
            empty_initialize();//创建一个header
            swap(rhs);//三个数据成员赋值
        }

        rb_tree &operator=(rb_tree &&rhs) noexcept{
            clear();// 当前结构被清空
            swap(rhs); //rhs全部移动过来
            return *this;
        }

    public: 
        //getter    固定的迭代器
        const_iterator begin() const noexcept{
            return leftmost();//开始节点就是最左下节点
        }
        const_iterator end() const noexcept{
            return header;//结束节点就是头节点
        }
        const_iterator cbegin() const noexcept{
            return leftmost();
        }
        const_iterator cend() const noexcept{
            return header;
        }

        const_reverse_iterator rbegin() const noexcept{
            return reverse_iterator(end());
        }
        const_reverse_iterator rend() const noexcept{
            return reverse_iterator(begin());
        }
        const_reverse_iterator crbegin() const noexcept{
            return reverse_iterator(end());
        }
        const_reverse_iterator crend() const noexcept{
            return reverse_iterator(begin());
        }

        bool empty() const noexcept{
            return node_count == 0;
        }
        size_type size() const noexcept{
            return node_count;
        }


    public:
        //setter   可以修改的迭代器
        iterator begin() noexcept{
            return leftmost;
        }
        iterator end() noexcept{
            return header;
        }
        reverse_iterator rbegin() noexcept{
            return reverse_iterator(end());            
        }
        reverse_iterator rend() noexcept{
            return reverse_iterator(begin());
        }

    private:
        //aux interface for insert   插入辅助接口
        iterator insert_aux(base_ptr, base_ptr, const value_type &);
    public:
        //insert
        pair<iterator, bool> insert_unique(const value_type &);
        iterator insert_unique(iterator, const value_type &);
        template<class InputIterator>
        void insert_unique(InputIterator , InputIterator);

        iterator insert_equal(iterator, const value_type&);
        iterator insert_equal(const value_type &);

        template <class InputIterator>
        void insert_equal(InputIterator, InputIterator);


    private:
        //erase interface
        void erase_aux(link_type) noexcept;
    
    public:
        erase(iterator);
        size_type erase(const key_type &);
        void erase(iterator, iterator);
        void clear() noexcept;
    
    private:
        //find
        iterator find(const key_type &) noexcept;
        const_iterator find(const key_type &) const noexcept;
        size_type count(const key_type &) const noexcept;
        iterator lower_bound(const key_type &) noexcept;
        const_iterator lower_bound(const key_type &) noexcept;
        iterator upper_bound(const key_type&) noexcept;
        const_iterator upper_bound(const key_type &) noexcept;
        pair<iterator, iterator> equal_range(const key_type &) noexcept;
        pair<const_iterator, const_iterator> equal_range(const key_type& ) const noexcept;
    public:
        //swap
        void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &lhs) noexcept {
            //swap data member;
            MiniSTL::swap(header, lhs.header);
            MiniSTL::swap(node_count, lhs.count);
            MiniSTL::swap(key_compare, lhs.key_compare);
        }
    };

    template <class Key, class Value, class KeyOfValue, class Compare , class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare,Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue,Compare,Alloc>::insert_aux(
        base_ptr x_, base_ptr y_, const value_type &val
    ){
        link_type x = reinterpret_cast<link_type>(x_);
        link_type y = reinterpret_cast<link_type>(y_);
        link_type z;
        if(y == header || x || key_compare(KeyOfValue()(val), key(y))){
            //待插入点之父为header || 待插入节点自身并不为nullptr，（何时触发）||父节点明确大于待插入值
            z = create_node(val);
            left(y) = z;//若y为header ， 此时leftmost = z
            if(y == header){
                root() = z;
                rightmost() = z;
            }else if(y == leftmost()){
                leftmost() = z;
            }
        }else{
            //此时必成为y右子
            z = create_node(val);
            right(y) = z;
            if(y == rightmost()){
                rightmost() = z;
            }
        }
        parent(z) = y;
        left(z) = nullptr;
        right(z) = nullptr;
        rb_tree_rebalance(z, header->parent);
        ++node_count;
        return iterator(z);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator , bool>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(
        const value_type &val
    ){
        // 插入新值，节点键值允许重复
        // 注意返回值是一个迭代器，指向新增节点
        //插入一个值为val的节点
        link_type y = header ;
        link_type x = root(); //从根节点开始，
        bool comp = true;
        while(x){  // x!= nullptr     向下寻找插入点
            y = x;//时刻保持是x的父亲节点
            comp = key_compare(KeyOfValue()(val), key(x));//value 是否小于x的键值
            x = comp ? left(x) : right(x); // val < x  x = x->left  
            //意思就是x值如果比val小，就一直向左遍历
        }
        // 此时x为nullptr   y是x的父亲
        //此时y必为待插入点的父节点（也必为叶节点）
        iterator j(y); //j = iterator(y); 
        if(comp){// val < y的键值    插于左侧
            if(j == begin()){//待插入点之父为最左节点
                //x---插入点    y----插入点之父   val---插入值
                return pair<iterator, bool>(insert_aux(x, y , val), true);
            }else{
                j--;//调整j准备完成测试（可能与某键值重复）
            }
        }
        if(key_compare(key(j.node), KeyOfValue()(val))){
            //新键值不与旧键值重复，放心插入
            return pair<iterator, bool>(insert_aux(x,y,val) , true);
        }
        //进行至此  表示新值一定与树中键值重复，那么就不应该插入新值
        return pair<iterator, bool>(j, false);//当前value为重复值
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(
        iterator pos, const value_type &val
    ){
        //插入新值，及诶单那键值不允许重复，若重复，则插入无效
        //注意返回值  第一个元素是个迭代器 ，指向新增节点，
        //在pos 处插入 val
        if(pos.node == header->left){ //仅有root
            //begin()    v < pos  左插
            if(size() > 0 && key_compare(KeyOfValue()(val), key(pos.node))){
                //新值插入点，插入点之父， 新值
                return insert_aux(pos.node, pos.node, val);
            }else{
                return insert_unique(val).first;
            }
        }else if(pos.node == header){ //仅有header
            //end()
            if(key_compare(key(rightmost()), KeyOfValue()(val))){
                //插入点nullptr  插入点之父 rightmost(==right)  插入值val
                return insert_aux(nullptr, rightmost(), val);
            }else{
                return insert_unique(val).first;
            }
        }else{
            //不是首尾点进行插入
            iterator before = pos;
            --before; 
            if(key_compare(key(before.node), KeyOfValue()(val)) && 
                key_compare(KeyOfValue()(val) , key(pos.node))){
                if(!right(before.node)){
                    return insert_aux(nullptr, before.node, val);
                }else{
                    return insert_aux(pos.node, pos.node, val);
                }
            }else{
                return insert_unique(val).first;
            }
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    template<class InputIterator>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(
        InputIterator first, InputIterator last
    ){
        for(; first != last ; first++){
            insert_unique(*first);//调用上面的插入单值的重载函数   
            //将一个区间的元素插入到红黑树中，
        }
    }

    //允许节点键值重复，
    //返回值是一个迭代器，指向新增节点
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename  rb_tree<Key, Value, KeyOfValue,Compare, Alloc>::iterator 
    rb_tree<Key, Value ,KeyOfValue, Compare, Alloc>::insert_equal(
        const value_type& val
    ){
        link_type y = header ;
        link_type x = root();
        while(x){
            y = x;
            x = key_compare(KeyOfValue()(val), key(x)) ? left(x) : right(x);
            //  val  < x -----x = x->left
        }
        return insert_aux(x , y ,val);//x 为新值插入点，y为其父
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(
        iterator pos, const value_type& val
    ){
        if(pos.node == header->left){
            //begin()     pos.node === root   仅有header和root点
            if(size() > 0 && !key_compare(key(pos.node) , KeyOfValue()(val))){
                //
                return insert_aux(pos.node, pos.node, value);
            }else{
                return insert_equal(val);
            }
        }else if(pos.node == header){
            if(! key_compare(KeyOfValue()(val)), key(rightmost()))
                return insert_aux(nullptr,rightmost() ,val);
            else
                return insert_equal(val);
        }else{
            iterator before = pos;
            --before;
            if(!key_compare(KeyOfValue()(val), key(before.node)) && !key_compare(key(pos.node), KeyOfValue()(val))){
                if(!right(before.node)){
                    return insert_aux(nullptr, before.node,val);
                }else{
                    insert_aux(pos.node, pos.node,val);
                }
            }else{
                return insert_equal(val);
            }
        }
    }

    template<class Key , class Value, class KeyOfValue, class Compare, class Alloc>
    template<class InputIterator>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(
        InputIterator first, InputIterator last
    ){
        for(; first != last; first++){
            insert_equal(*first);
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator pos){
        link_type y = reinterpret_cast<link_type>((rb_tree_rebalance_for_erase(
            pos.node, header->parent, header->left, header->right
        ));
        destory_node(y);
        --node_count;
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const key_type &k){
        pair<iterator, iterator> p = equal_range(k);
        size_type n = MiniSTL::distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }     

    template<class Key, class Value, class KeyOfValue, class  Compare, class Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, iterator last){
        if(first == begin() && last == end()){
            clear();
        }else{
            while(first != last){
                erase(first++);
            }
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::clear() noexcept{
        if(node_count){
            erase_aux(root());
            leftmost() == header;
            root() = nullptr;
            rightmost() = header;
            node_count = 0;
        }
    }

} // namespace MiniSTL
