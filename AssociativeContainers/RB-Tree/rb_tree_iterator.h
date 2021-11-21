#pragma once

#include <cstddef>
#include "rb_tree_node.h"
#include "stl_iterator.h"

namespace MiniSTL
{
    struct rb_tree_base_iterator
    {
        using base_ptr = __rb_tree_node_base::base_ptr;
        using iterator_category = bidirectional_iterator_tag;//双向移动的指针
        using difference_type = ptrdiff_t;

        base_ptr  node;

        void increment(){//正向 移动   二叉搜索树中 下一个节点 就是下一个比自己大的节点（右存在  找柚子的最左）
            //右节点不存在  需要找父亲，如果当前节点是父亲节点的右节点，需要不断上溯
            if(node->right != nullptr){
                //存在右子，则下一节点必为右子树的最左下角
                node = node->right;
                while(node->left != nullptr){
                    node = node->left;
                }
            }else{
                base_ptr p = node->parent;
                while(node == p->right){
                    //不断上溯，直到找到第一个不为右子的祖先
                    node = p;
                    p = p->parent;
                }//直到不是右节点为止

                //p 是父亲  node是当前
                //正常情况下该祖先之父即为结束
                if(node->right != p){//若此时的右节点不等于此时的父节点即为解答，否则node为解答
                //这样做的目的是为了应付一种特殊情况，我们欲寻找根节点的下一个节点。而恰巧根节点无右孩子
                // 当然，以上特殊做法必须配合rb_tree根节点与特殊header之间的特殊关系
                //此时的header和root互为对方的父节点
                    node = p;
                }
                //若右子等于父节点，node即为自增结果
                //此为特殊情况，使用于迭代器指向root而后者无right
            }
        }

        //header的左右节点都是根节点

        void decrement(){
            if(node->color == rb_tree_red && node->parent->parent == node){//互为父亲  就是自己（只有header有这个特性）
                //此为特例  当迭代器指向end()将触发此情况
                //当node为header的时候，(node为end())  注意header之右孩子即为mostright，指向整棵树的max节点
                node = node->right;//header的下一个节点就是right
            }else if(node->left != nullptr){
                //存在左子  ， 前一节点必然是左子树的最右下角
                base_ptr left = node->left;
                while(left->right != nullptr){
                    left = left->right;
                }
                node = left;
            }else{//左子树为空  是一个单支的右叉树，需要找到比当前节点大的，如果当前节点的是父亲节点的左子树
            //那父亲还是小点，再继续找，直到父亲的左节点不是当前节点为止，那么此时的父亲就是答案
                base_ptr p = node->parent;
                while(node == p->left){//不断上溯，直至找到第一个不为左子的祖先
                    node = p;
                    p = p->parent;
                }
                node = p;//该祖先的父亲即为答案
            }
        }
    };


    template<class T, class Ref, class Ptr>
    struct rb_tree_iterator: public rb_tree_base_iterator
    {
        using value_type = T;
        using reference = Ref;
        using pointer = Ptr;
        using iterator = rb_tree_iterator<T,T&,T*>;
        using const_iterator = rb_tree_iterator<T,const T&, const T*>;
        using self = rb_tree_iterator<T,Ref, Ptr>;
        using link_type = __rb_tree_node<T*>;

        rb_tree_iterator(){}
        rb_tree_iterator(link_type x) {node = x;}
        rb_tree_iterator(const iterator &it){
            node = it.node;
        }
        reference operator*()const { //取其内容
            return reinterpret_cast<link_type>(node)->value_field;
        }
        pointer operator->() const {
            return &(operator*());
        }

        self& operator++(){//z++
            increment();
            return *this;
        }

        self operator++(int){
            self temp = *this;//++z
            increment();
            return temp;
        }

        self& operator--(){
            decrement();
            return *this;
        }
        self operator--(int){
            self temp = *this;
            decrement();
            return temp;
        }
    };

//两个迭代器相等，意味着他们指向同一个红黑树节点
    inline bool operator==(const rb_tree_base_iterator &lhs , const rb_tree_base_iterator &rhs){
        return lhs.node == rhs.node;
    }

    inline bool operator!=(const rb_tree_base_iterator &lhs, const rb_tree_base_iterator &rhs){
        return lhs.node != rhs.node;
    }
} // namespace MiniSTL
