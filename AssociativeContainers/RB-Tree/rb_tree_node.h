#pragma once
 namespace MiniSTL{

     using rb_tree_color_type = bool ;
     const rb_tree_color_type rb_tree_red = false;
     const rb_tree_color_type rb_tree_black = true; //黑true

     struct __rb_tree_node_base
     {
         //alias declaration
         using color_type = rb_tree_color_type;//bool 
         using base_ptr = __rb_tree_node_base*;//next

        //data member
        color_type color;//bool
        base_ptr parent;//node*
        base_ptr left;//node* left
        base_ptr right;//node* right

        //get min/max ptr
        //二叉搜索树  一直向右  为max
        static base_ptr maximum(base_ptr root){
            while(root->right){//right != nullptr   root->right   最右下的叶子
                root = root->right;
            }
            return root;
        }
        static base_ptr minium(base_ptr root){
            while (root->left)
            {
                root = root->left;
            }
            return root;
        }
     };

//红黑树的结构：
// 父指针，
// 左右孩子指针
// 节点值
     template<class T>
     struct __rb_tree_node : public __rb_tree_node_base{
         using link_type = __rb_tree_node<T>* ;//next
         T value_field;//value
     };

     
}